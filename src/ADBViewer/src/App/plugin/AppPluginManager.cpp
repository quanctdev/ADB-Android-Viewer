
#include "../../ADBViewer.h"

#if defined(OS_WIN)
#  include "IPluginLoader.WINAPI.h"
#else
#  include "IPluginLoader.LINUX.h"
#endif

using namespace std::placeholders;

namespace Plugins
{

typedef Plugins::AppIPlugin * (*f_Fn_construct)(const char*, const void*);
typedef void (*f_Fn_destruct)(void);

static inline const char *l_nameCreatePlugin = "CreatePlugin";
static inline const char *l_nameDestroyPlugin = "DestroyPlugin";

AppPluginManager::AppPluginManager()
        : m_isrun(false)
    {
        m_loadfunc = std::make_unique<Plugins::PluginLoader>();
    }

AppPluginManager::~AppPluginManager()
    {
        m_isrun = false;

        if (m_thr.joinable())
            m_thr.join();

        m_isrun = true;
        freeplugins();
    }

AppPluginManager& AppPluginManager::instance()
    {
        static AppPluginManager m_instance{};
        return m_instance;
    }

void AppPluginManager::init()
    {
        if (m_plugins.size())
            return;

        m_isrun = true;

        std::thread thr
        {
            [=]()
            {
                m_loadfunc->PluginFind(
                        std::bind(&Plugins::AppPluginManager::addplugin, this, _1, _2, _3)
                    );

                if (!m_isrun.load())
                    return;

                if (m_plugins.size() > 1U)
                {
                    std::sort(
                            m_plugins.begin(), m_plugins.end(),
                            [=](const auto& lplg, const auto& rplg)
                            {
                                if (!lplg.iplug)
                                    return false;
                                return (lplg.iplug->priority() < rplg.iplug->priority());
                            }
                        );
                }
                m_isrun = false;
            }
        };
        m_thr = move(thr);
    }

void AppPluginManager::run(std::vector<uint8_t> &v, uint32_t w, uint32_t h)
    {
        if (m_isrun.load())
            return;

        if (m_thr.joinable())
            m_thr.join();

        if (!m_plugins.size())
            return;

        /// lock
        {
            bool isready = false;

            for (auto &plg : m_plugins)
            {
                if ((!plg.state) && (plg.handle))
                    disableplugin(plg);
                if ((!plg.state) || (!plg.iplug))
                    continue;
                {
                    std::lock_guard<std::mutex> l_lock(m_lock);
                    if ((isready = plg.iplug->ready()))
                        break;
                }
            }
            if (!isready)
                return;
        }

        m_isrun = true;

        std::vector<uint8_t> vt(v);
        std::thread thr
        {
            [=]()
            {
                for (auto &plg : m_plugins)
                {
                    if (!m_isrun.load())
                        break;
                    if ((!plg.state) || (!plg.iplug))
                        continue;
                    {
                        std::lock_guard<std::mutex> l_lock(m_lock);
                        if (plg.iplug->ready())
                            plg.iplug->go(vt, w, h);
                    }
                }
                m_isrun = false;
            }
        };
        m_thr = move(thr);
    }

bool AppPluginManager::isplugin(std::string const & s)
    {
        auto plg = find_if(
                    m_plugins.begin(),
                    m_plugins.end(),
                    [=](Plugin_s pl)
                    {
                        return (pl.name.compare(0, pl.name.length(), s) == 0);
                    }
            );
        return (plg != m_plugins.end());
    }

AppPluginManager::Plugin_s * AppPluginManager::findplugin(std::string const & s)
    {
        auto plg = find_if(
                    m_plugins.begin(),
                    m_plugins.end(),
                    [=](AppPluginManager::Plugin_s pl)
                    {
                        return (pl.name.compare(s) == 0);
                    }
            );
        if (plg == m_plugins.end())
            return nullptr;
        return &plg[0];
    }

void AppPluginManager::addplugin(std::string const & sp, std::string const & sn, bool isenable)
    {
        {
            std::lock_guard<std::mutex> l_lock(m_lock);
            if (isplugin(sn))
                return;
        }

        AppPluginManager::Plugin_s plg{};
        plg.state = false;

#       if defined(OS_WIN)
        int32_t off = 4;
#       elif defined(__APPLE__)
        int32_t off = 6;
#       else
        int32_t off = 3;
#       endif

        std::stringstream ss;
        ss << sp.c_str() << "\\" << sn.c_str();
        plg.path = ss.str();
        plg.name = sn.substr(0, sn.length() - off);

        if (isenable)
        {
            if (enableplugin(plg))
                m_plugins.push_back(plg);
        }
        else
        {
            plg.state = false;
            plg.handle = nullptr;
            plg.iplug = nullptr;
            m_plugins.push_back(plg);
        }
    }

bool AppPluginManager::enableplugin(AppPluginManager::Plugin_s & plg)
    {
        do
        {
            std::lock_guard<std::mutex> l_lock(m_lock);

            if (plg.handle)
                disableplugin(plg);

            if ((plg.path.empty()) || (plg.name.empty()))
                break;

            if (!(plg.handle = m_loadfunc->PluginOpen(plg.path.c_str())))
                break;

            f_Fn_construct func = (f_Fn_construct)
                m_loadfunc->PluginInstance(plg.handle, l_nameCreatePlugin);
            if (!func)
                break;

            if (!(plg.iplug = func(plg.name.c_str(), AppConfig::instance().GetAdbCb())))
                break;

            plg.state = true;
            return plg.state;
        }
        while (0);

        disableplugin(plg);
        return plg.state;
    }

void AppPluginManager::disableplugin(AppPluginManager::Plugin_s & plg) noexcept
    {
        std::lock_guard<std::mutex> l_lock(m_lock);
        plg.state = false;

        if (plg.iplug)
        {
            f_Fn_destruct func = (f_Fn_destruct)
                m_loadfunc->PluginInstance(plg.handle, l_nameDestroyPlugin);
            if (func)
                func();
        }
        if (plg.handle)
            m_loadfunc->PluginClose(plg.handle);

        plg.iplug = nullptr;
        plg.handle = nullptr;
    }

void AppPluginManager::disableplugin(std::string const & s)
    {
        AppPluginManager::Plugin_s *plg;
        {
            std::lock_guard<std::mutex> l_lock(m_lock);
            plg = findplugin(s);
        }
        if (!plg)
            return;
        if (m_isrun)
            plg->state = false;
        else
            disableplugin(*plg);
    }

bool AppPluginManager::enableplugin(std::string const & s)
    {
        AppPluginManager::Plugin_s *plg;
        {
            std::lock_guard<std::mutex> l_lock(m_lock);
            plg = findplugin(s);
        }
        if (!plg)
            return false;
        return enableplugin(*plg);
    }

std::vector<AppPluginManager::Plugin_s> AppPluginManager::listplugin() const
    {
        return m_plugins;
    }

void AppPluginManager::freeplugins() noexcept
    {
        if (!m_plugins.size())
            return;

        for (auto &plg : m_plugins)
            disableplugin(plg);
    }
}
