/*
    MIT License

    Android remote Viewer, GUI ADB tools

    Android Viewer developed to view and control your android device from a PC.
    ADB exchange Android Viewer, support scale view, input tap from mouse,
    input swipe from keyboard, save/copy screenshot, etc..

    Copyright (c) 2016-2019 PS
    GitHub: https://github.com/ClnViewer/ADB-Android-Viewer

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sub license, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */

#include "../ADBViewer.h"
#include <ctime>

static inline const LPCSTR l_openLuaFilter = "Lua script files (*.lua)\0*.lua\0";
static inline const LPCSTR l_openLuaExt = "lua";
static inline const LPCSTR l_openCurDir = ".\\";
static inline const char   l_savePath[] = ".\\plugins\\plugin-lua.lua";

AppEditor::AppEditor()
    : ppixel{}, scrtype(SCR_CHECK_AND_CLICK),
      m_app{nullptr}, m_enable(false), m_target(false)
    {
        ppixel.x = ppixel.y = -1;
    }

AppEditor::~AppEditor()
    {
        guiBase::ActiveOff();
    }

bool AppEditor::isenabled() const
    {
        return m_enable.load();
    }

bool AppEditor::istarget() const
    {
        return m_target.load();
    }

bool AppEditor::isupdate() const
    {
        return ((m_enable.load()) ?
                ((ppixel.x > -1) && (ppixel.y > -1)) : false);
    }

bool AppEditor::init(App *app)
    {
        if (!app)
            return false;

        m_app = app;
        guiBase::gui.rdst = {};
        guiBase::gui.rsrc = nullptr;
        guiBase::gui.texture = nullptr;

        bool ret = guiBase::initgui(app);
        if (ret)
        {
            SDL_Rect rect{};
            SDL_Point point_img_menu = ResManager::imagesize(
                    ResManager::IndexImageResource::RES_IMG_MENU_ACTIVE
            );

            rect.x = 0;
            rect.y = (AppConfig::instance().cnf_disp_point.y - point_img_menu.x);

            ret = m_icon_record.init(
                    app,
                    rect,
                    ResManager::IndexImageResource::RES_IMG_RCORD,
                    [=](SDL_Event *ev, SDL_Rect*)
                    {
                        switch (ev->type)
                        {
                            case SDL_MOUSEMOTION:
                                {
                                    AddMessageQueue(
                                        ResManager::stringload(
                                            ResManager::IndexStringResource::RES_STR_SCRIPTEDIT_END,
                                            AppConfig::instance().cnf_lang
                                        ),
                                        5U,
                                        (__LINE__ + 1001)
                                    );
                                    return true;
                                }
                            case SDL_MOUSEBUTTONDOWN:
                                {
                                    guiBase::PushEvent(ID_CMD_POP_MENU6);
                                    return true;
                                }
                        }
                        return false;
                    }
                );

        }
        return ret;
    }

void AppEditor::stop()
    {
        if ((!m_app) || (!m_enable.load()))
            return;

        m_enable = false;
        m_target = false;

        m_icon_record.Off();
        m_app->m_appmsgbar.clean();
        guiBase::PushEvent(ID_CMD_POP_MENU102);
    }

void AppEditor::savetofile()
    {
        std::string fname(
            ResManager::stringload(
                ResManager::IndexStringResource::RES_STR_LUAFILENAME,
                AppConfig::instance().cnf_lang
            )
        );

        if (!AppSysDialog::savefile(
                    guiBase::GetGui<SDL_Window>(),
                    fname,
                    l_openLuaFilter,
                    l_openLuaExt,
                    l_openCurDir
                ))
                return;

        if (fname.empty())
            return;

        std::string data = getscript();
        if (data.empty())
            return;

        FILE __AUTO(__autofile) *fp = fopen(fname.c_str(), "wt");
        if (!fp)
            return;
        fwrite(data.c_str(), 1, data.length(), fp);

        while (0);
    }

void AppEditor::openedit()
    {
        std::string data = getscript();
        if (data.empty())
            return;

        if (!RunEdit(l_savePath, data, false, nullptr))
            AddMessageQueue(
                ResManager::stringload(
                    ResManager::IndexStringResource::RES_STR_SCRIPTEDIT_NOTRUN,
                    AppConfig::instance().cnf_lang
                ),
                5U,
                -1
            );
    }

void AppEditor::loadedit()
    {
        if (!RunEditOpen(l_savePath, false, nullptr))
            AddMessageQueue(
                ResManager::stringload(
                    ResManager::IndexStringResource::RES_STR_SCRIPTEDIT_NOTRUN,
                    AppConfig::instance().cnf_lang
                ),
                5U,
                -1
            );
    }

void AppEditor::run()
    {
        if ((!m_app) || (m_enable.load()))
            return;

        m_pixels.clear();
        m_ptarget.clear();
        ppixel.x = ppixel.y = -1;

        m_icon_record.On();

        m_target = false;
        m_enable = true;
        m_app->m_appmsgbar.clean();
        guiBase::PushEvent(ID_CMD_POP_MENU102);
    }

void AppEditor::cancel()
    {
        if ((!m_app) || (!m_enable.load()))
            return;

        m_icon_record.Off();

        m_enable = false;
        m_target = false;
        m_pixels.clear();
        m_ptarget.clear();
        m_app->m_appmsgbar.clean();
        guiBase::PushEvent(ID_CMD_POP_MENU102);
    }

void AppEditor::update(uint32_t w, uint32_t h, std::vector<uint8_t> & v)
    {
        if ((!m_app) || (!m_enable.load()))
            return;

        do
        {
            if (!isupdate())
                break;

            do
            {
                bool    iswriteinfo = false;
                int32_t x = ppixel.x,
                        y = ppixel.y,
                        p = ((m_target) ? 4 : 2),
                        z = ((m_target) ? 9 : 5);

                x = (((x + p) > (int32_t)w) ?
                     ((int32_t)w - z) :
                        (((x - p) < 0) ? 0 : (x - p))
                    );
                y = (((y + p) > (int32_t)h) ?
                     ((int32_t)h - z) :
                        (((y - p) < 0) ? 0 : (y - p))
                    );

                if (m_target)
                    m_ptarget.clear();

                for (int32_t xx = 0; xx < z; xx++)
                    for (int32_t yy = 0; yy < z; yy++)
                    {
                        if ((!AppConfig::instance().cnf_isrun) || (AppConfig::instance().cnf_isstop))
                            return;

                        AppEditor::_PIXELS pixel{};
                        pixel.x = (x + xx);
                        pixel.y = (y + yy);
                        pixel.pos = getpos(pixel.x, pixel.y, w, h);
                        if ((pixel.pos < 0) || ((size_t)pixel.pos >= v.size()))
                            continue;

                        if (m_target)
                        {
                            if (!foundpos(m_ptarget, pixel))
                                continue;
                        }
                        else
                        {
                            if (!foundpos(m_pixels, pixel))
                                continue;
                        }

                        AppEditor::_RGB *rgb = (AppEditor::_RGB*)&v[pixel.pos];
                        pixel.rgb.r = rgb->r;
                        pixel.rgb.g = rgb->g;
                        pixel.rgb.b = rgb->b;

                        if (m_target)
                            m_ptarget.push_back(pixel);
                        else
                            m_pixels.push_back(pixel);

                        /// Info position, x/y, color RGB
                        if ((!iswriteinfo) && (yy == (z / 2)))
                        {
                            std::stringstream ss;
                            ss << " P: "  << std::to_string(pixel.pos);
                            ss << " X: "  << std::to_string(pixel.x);
                            ss << " Y: "  << std::to_string(pixel.y);
                            ss << " ( R:" << std::to_string(pixel.rgb.r);
                            ss << ", G:"  << std::to_string(pixel.rgb.g);
                            ss << ", B:"  << std::to_string(pixel.rgb.b) << " ) ";
                            AddMessageQueue(
                                    ss.str(),
                                    5U,
                                    (__LINE__ + pixel.pos)
                                );
                            iswriteinfo = true;
                        }
                    }
            }
            while (0);

            ppixel.x = ppixel.y = -1;
        }
        while (0);

        AppEditor::_RGB const *rgb_;

        switch (scrtype)
        {
            case AppEditorScriptType::SCR_CHECK_AND_CLICK:
                {
                    rgb_ = &m_redrgb;
                    break;
                }
            case AppEditorScriptType::SCR_CLICK_ONLY:
                {
                    rgb_ = &m_bluergb;
                    break;
                }
            default:
                {
                    rgb_ = &m_greenrgb;
                    break;
                }
        }

        if ((!AppConfig::instance().cnf_isrun) || (AppConfig::instance().cnf_isstop))
            return;

        for (auto &rgbs : m_pixels)
            ::memcpy(&v[rgbs.pos], rgb_, sizeof(m_redrgb));
        for (auto &rgbs : m_ptarget)
            ::memcpy(&v[rgbs.pos], &m_bluergb, sizeof(m_bluergb));
    }

int32_t AppEditor::getpad(uint32_t w)
    {
        int32_t pad = 0;
        while ((((int32_t)w + pad) % 4) != 0)
            pad++;
        return pad;
    }

int32_t AppEditor::checkpos(uint32_t w, uint32_t h, int32_t pad)
    {
        return (((((int32_t)w * 3) + pad) * (int32_t)h) - 3);
    }

int32_t AppEditor::getpos(int32_t x, int32_t y, uint32_t w, uint32_t h)
    {
        int32_t pad = getpad(w),
                pos = (((((int32_t)w * 3) + pad) * y) + (x * 3));
        if (pos > checkpos(w, h, pad))
            return -1;
        return pos;
    }

bool AppEditor::foundpos(std::vector<AppEditor::_PIXELS> & v, AppEditor::_PIXELS & p)
    {
        auto a = find_if(
                    v.begin(),
                    v.end(),
                    [=](AppEditor::_PIXELS pix)
                    {
                        return pix.pos == p.pos;
                    }
            );
        if (a != v.end())
        {
            v.erase(a);
            return false;
        }
        return true;
    }

std::string AppEditor::getscript()
    {
        SDL_Rect *r = m_app->m_appvideo.guiBase::GetGui<SDL_Rect>();
        int32_t sc = static_cast<int32_t>(AppConfig::instance().cnf_disp_ratio);
        std::time_t tnow = std::time(NULL);
        std::stringstream ss;
        ss << "\n -- Android ADB Viewer " AVIEW_FULLVERSION_STRING " - " AVIEW_DATE "/" AVIEW_MONTH "/" AVIEW_YEAR;
        ss << "\n -- Display resolution: " << r->w << "x" << r->h;
        ss << "\n -- Date build script: " << std::ctime(&tnow);
        ss << "\n\nfunction main (stateOld)\n";

        switch (scrtype)
        {
            case AppEditorScriptType::SCR_CHECK_AND_CLICK:
                {
                    ss << "\tlocal tbl_screen01 = {\n";

                    for (auto &rgbs : m_pixels)
                        ss << "\t\t{" << (int)rgbs.pos << "," << (int)rgbs.rgb.r << "," << (int)rgbs.rgb.g << "," << (int)rgbs.rgb.b << "},\n";
                    ss << "\t}\n";

                    if (m_ptarget.size())
                    {
                        int32_t pos = (m_ptarget.size() / 2);
                        ss << "\tlocal ret = LuaObject:checkPixelsByPos(tbl_screen01)\n";
                        ss << "\tif ret then\n";
                        ss << "\t\tLuaObject:adbClick(";
                        ss << (m_ptarget[pos].x * sc) << "," ;
                        ss << (m_ptarget[pos].y * sc) << ")\n";
                        ss << "\tend\n";
                    }
                    ss << "\n\tLuaObject:stateSleep(10)\n";
                    break;
                }
            case AppEditorScriptType::SCR_CLICK_ONLY:
                {
                    int32_t cnt = 0,
                            idx = 0;
                    for (auto &rgbs : m_pixels)
                    {
                        if (idx == 24)
                        {
                            idx = 0;
                            continue;
                        }
                        if (idx != 12)
                        {
                            idx++;
                            continue;
                        }
                        idx++;

                        if (!cnt)
                            ss << "\t";
                        else
                            ss << "\telse";

                        ss << "if stateOld == " << cnt++ << " then LuaObject:adbClick(";
                        ss << (rgbs.x * sc) << "," ;
                        ss << (rgbs.y * sc) << ")\n";
                    }
                    ss << "\telseif stateOld == " << cnt << " then stateOld = -1 end\n";
                    ss << "\n\tLuaObject:stateSleep(1)\n";
                    break;
                }
            case AppEditorScriptType::SCR_NONE:
                {
                    break;
                }
        }
        ss << "\tLuaObject:stateSet(stateOld + 1)\n";
        ss << "end\n\n";

        if (m_ptarget.size())
            m_ptarget.clear();

        if (m_pixels.size())
            m_pixels.clear();

        return ss.str();
    }

bool AppEditor::uevent(SDL_Event *ev, const void *instance)
    {
        AppEditor *ape = static_cast<AppEditor*>(
                const_cast<void*>(instance)
            );

        if (!ape)
            return false;

        switch(ev->user.code)
        {
            case ID_CMD_POP_MENU6:
                {
                    /// (trigger)
                    if (ape->isenabled())
                    {
                        /// stop and write LUA script
                        ape->stop();
                        ape->savetofile();
                    }
                    else
                        /// start edit pixels 5x5 for check && identify
                        ape->run();
                    return true;
                }
            case ID_CMD_POP_MENU66:
                {
                    /// (trigger)
                    if (ape->isenabled())
                    {
                        /// stop and open LUA script from editor
                        ape->stop();
                        ape->openedit();
                    }
                    return true;
                }
            case ID_CMD_POP_MENU67:
                {
                    /// open default LUA script from editor
                    ape->loadedit();
                    return true;
                }
            case ID_CMD_POP_MENU7:
                {
                    if (ape->isenabled())
                        /// cancel and clear pixels list
                        ape->cancel();
                    return true;
                }
            case ID_CMD_POP_MENU8:
                {
                    /// (trigger)
                    /// add endpoint action
                    m_target = !m_target;
                    return true;
                }
            default:
                break;
        }
        return false;
    }

bool AppEditor::event(SDL_Event *ev, const void *instance)
    {
        AppEditor *ape = static_cast<AppEditor*>(
                const_cast<void*>(instance)
            );

        if (!ape)
            return false;

        if (!ape->isenabled())
            return false;

        if ((ev->type == SDL_RENDER_TARGETS_RESET) || (ev->type == SDL_RENDER_DEVICE_RESET))
            return false;

        if (!ape->guiBase::IsRegion(ev, ape->m_app->m_appvideo.guiBase::GetGui<SDL_Rect>()))
            return false;

        switch(ev->type)
        {
            case SDL_MOUSEBUTTONDOWN:
            {
                switch (ev->button.button)
                {
                    case SDL_BUTTON_LEFT:
                    {
                        SDL_Point point_img_menu = ResManager::imagesize(
                            ResManager::IndexImageResource::RES_IMG_MENU_ACTIVE
                        );

                        if (ev->motion.x <= point_img_menu.x)
                            break;

                        if (ape->isupdate())
                            break;

                        ape->ppixel.x = (ev->motion.x - point_img_menu.x);
                        ape->ppixel.y = ev->motion.y;
                        return true;
                    }
                    default:
                        break;
                }
                break;
            }
            default:
                break;
        }
        return false;
    }

