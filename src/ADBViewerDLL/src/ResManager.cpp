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

#include "../../ADBViewer/src/loader.h"
#include "ResManager.h"
#include <cstdlib>
#include <ctime>

namespace Resources
{
#include "Resources/ResHelp_ru.h"
#include "Resources/ResHelp_en.h"
#include "Resources/ResHelp_dm.h"
#include "Resources/ResHelp_cn.h"
#include "Resources/ResLogo.h"
#include "Resources/ResMenu0.h"
#include "Resources/ResMenu1.h"
#include "Resources/ResScreen.h"
#include "Resources/ResRecord.h"
#include "Resources/ResEditMenu.h"
#include "Resources/ResTermClose.h"
#include "Resources/Res16704font.h"
#include "Resources/ResConsolafont.h"
#include "Resources/ResFreeSansfont.h"
#include "Resources/clip/bender-anime-8/bender8sprite.h"
#include "Resources/clip/bender-anime-8/bender8speech.h"
#include "Resources/clip/browser-menu4/browser_menu4sprite.h"
#include "Resources/clip/browser-menu9/browser_menu9sprite.h"
#include "Resources/clip/keyboard-active/keyboard_active2sprite.h"

typedef struct
{
    TTF_Font  *font;
    SDL_RWops *rwops;

} font_cache_s;

typedef struct
{
    const Resources::ResManager::ImageResource_s **sp;
    uint32_t   sz;

} sprites_base_s;

static inline SDL_Color box_color[][2] =
{
    {
        { 191, 227, 103, 0 },
        { 2, 2, 2, 150 }
    },
    {
        { 5, 5, 5, 0 },
        { 250, 250, 250, 150 }
    },
    {
        { 151, 192, 36, 0 },
        { 0, 0, 0, 0 }
    },
    {
        { 104, 192, 0, 0 },
        { 155, 204, 38, 0 }
    },
    {
        { 255, 255, 255, 255 },
        { 0, 0, 0, 255 }
    },
    {
        { 187, 229, 59, 255 },
        { 0, 0, 0, 255 }
    },
    {
        { 255, 255, 255, 255 },
        { 151, 192, 36, 0 }
    }
};

static font_cache_s font_cache[3]{};

static const ResManager::ImageResource_s *image_base[] =
{
    &img_logo,
    &img_menu0,
    &img_menu1,
    &img_screen,
    &img_record,
    &img_termclose,
    &img_editmenu
};

static sprites_base_s sprite_base[] =
{
    {
        img_bender8sprites,
        __NELE(img_bender8sprites)
    },
    {
        img_menu4sprites,
        __NELE(img_menu4sprites)
    },
    {
        img_menu9sprites,
        __NELE(img_menu9sprites)
    },
    {
        img_keyboard_active2sprites,
        __NELE(img_keyboard_active2sprites)
    }
};

__attribute__((constructor)) static void initialize_ResourcesRand()
{
    std::srand(unsigned(std::time(0)));
}

ResManager::ResManager() {}
ResManager::~ResManager() {}

const char * ResManager::stringload(ResManager::IndexStringResource idx, ResManager::IndexLanguageResource lang)
{
    if ((uint32_t)idx >= __NELE(help_strings_ru))
        return nullptr;

    switch(lang)
    {
        case ResManager::IndexLanguageResource::LANG_RU: return help_strings_ru[idx];
        case ResManager::IndexLanguageResource::LANG_EN: return help_strings_en[idx];
        case ResManager::IndexLanguageResource::LANG_DM: return help_strings_dm[idx];
        case ResManager::IndexLanguageResource::LANG_CN: return help_strings_cn[idx];
        default: return help_strings_ru[idx];
    }
}

const char * ResManager::speechrandom()
    {
        return txt_bender8speech[(std::rand() % __NELE(txt_bender8speech))];
    }

SDL_Surface ** ResManager::spriteload(ResManager::IndexSpriteResource idx, SDL_Color *transparent, uint32_t *psz)
    {
        do
        {
            if (psz)
                *psz = 0U;

            if ((idx < ResManager::IndexSpriteResource::RES_SPRITE_BENDER) ||
                (idx >= ResManager::IndexSpriteResource::RES_SPRITE_END))
                return nullptr;

            uint32_t i = 0U,
                     l_ssz = sprite_base[idx].sz;
            const ResManager::ImageResource_s **l_res = sprite_base[idx].sp;

            SDL_Surface **surf = new SDL_Surface*[l_ssz]{};
            if (!surf)
                break;

            for (; i < l_ssz; i++)
            {
                if (!(surf[i] = ResManager::imagedata(l_res[i])))
                    break;

                if (transparent)
                {
                    surf[i]->format->Amask = 0xFF000000;
                    surf[i]->format->Ashift = 24;

                    SDL_SetColorKey(
                        surf[i],
                        SDL_TRUE,
                        SDL_MapRGB(
                            surf[i]->format,
                            transparent->r,
                            transparent->g,
                            transparent->b
                        )
                    );
                }
            }

            if (i != l_ssz)
                break;

            if (psz)
                *psz = l_ssz;

            return surf;
        }
        while (0);

        return nullptr;
    }

SDL_Point ResManager::spritesize(ResManager::IndexSpriteResource idx)
{
    if ((idx < ResManager::IndexSpriteResource::RES_SPRITE_BENDER) ||
        (idx >= ResManager::IndexSpriteResource::RES_SPRITE_END) ||
        (!sprite_base[idx].sz))
        return { 0, 0 };

    return
    {
        static_cast<int32_t>(sprite_base[idx].sp[0]->w),
        static_cast<int32_t>(sprite_base[idx].sp[0]->h)

    };
}

SDL_Surface * ResManager::imageload(ResManager::IndexImageResource idx)
{
    if ((idx < ResManager::IndexImageResource::RES_IMG_LOGO) ||
        (idx >= ResManager::IndexImageResource::RES_IMG_END))
        return nullptr;

    return imagedata(image_base[idx]);
}

SDL_Point ResManager::imagesize(ResManager::IndexImageResource idx)
{
    if ((idx < ResManager::IndexImageResource::RES_IMG_LOGO) ||
        (idx >= ResManager::IndexImageResource::RES_IMG_END))
        return { 0, 0 };

    return
    {
        static_cast<int32_t>(image_base[idx]->w),
        static_cast<int32_t>(image_base[idx]->h)

    };
}

SDL_Surface * ResManager::imagedata(const ResManager::ImageResource_s *res)
{
    if (!res)
        return nullptr;

    uint32_t rmask, gmask, bmask, amask;
#   if SDL_BYTEORDER == SDL_BIG_ENDIAN
    int shift = (res->bpp == 3) ? 8 : 0;
    rmask = 0xff000000 >> shift;
    gmask = 0x00ff0000 >> shift;
    bmask = 0x0000ff00 >> shift;
    amask = 0x000000ff >> shift;
#   else // little endian, like x86
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = (res->bpp == 3) ? 0 : 0xff000000;
#   endif

    return SDL_CreateRGBSurfaceFrom(
                static_cast<void*>(res->data),
                res->w,
                res->h,
                (res->bpp * 8),
                (res->bpp * res->w),
                rmask, gmask, bmask, amask
            );
}

SDL_Surface * ResManager::loadbmp(char const *path)
{
    SDL_Surface *image_surface = SDL_LoadBMP(path);

    if(!image_surface)
        return nullptr;

    return image_surface;
}

void ResManager::fontcachefree()
{
    for (uint32_t i = 0U; i < __NELE(font_cache); i++)
    {
        Resources::ResManager::fontcachefree(
                static_cast<Resources::ResManager::IndexFontResource>(i)
            );
    }
}

void ResManager::fontcachefree(ResManager::IndexFontResource idx)
{
    if (font_cache[idx].font)
        TTF_CloseFont(font_cache[idx].font);
    if (font_cache[idx].rwops)
        SDL_RWclose(font_cache[idx].rwops);

    font_cache[idx].font = nullptr;
    font_cache[idx].rwops = nullptr;
}

TTF_Font * ResManager::fontload(ResManager::IndexFontResource idx)
{
    SDL_RWops *rwops = nullptr;
    TTF_Font  *font = nullptr;

    do
    {
        if (idx >= __NELE(font_cache))
            break;

        if (font_cache[idx].font)
        {
            return font_cache[idx].font;
        }
        if (!(rwops = ResManager::fontloadraw(idx)))
            break;
        if (!(font = TTF_OpenFontRW(rwops, 1, 16)))
            break;

        font_cache[idx].font = font;
        font_cache[idx].rwops = rwops;
        return font;
    }
    while (0);

    if (rwops)
        SDL_RWclose(rwops);

    return nullptr;
}

SDL_RWops   * ResManager::fontloadraw(ResManager::IndexFontResource idx)
{
    switch (idx)
    {
        case ResManager::IndexFontResource::RES_FONT_16704:
            {
                return SDL_RWFromMem(&data_font16704[0], __NELE(data_font16704));
            }
        case ResManager::IndexFontResource::RES_FONT_FREESANS:
            {
                return SDL_RWFromMem(&data_fontFreeSans[0], __NELE(data_fontFreeSans));
            }
        case ResManager::IndexFontResource::RES_FONT_CONSOLAS:
            {
                return SDL_RWFromMem(&data_fontConsolas[0], __NELE(data_fontConsolas));
            }
        default:
            {
                return nullptr;
            }
    }
}

SDL_Color * ResManager::colorload(ResManager::IndexColorResource idx)
{
    if ((idx < ResManager::IndexColorResource::RES_COLOR_GREEN_BLACK) ||
        (idx > ResManager::IndexColorResource::RES_COLOR_TERMINAL_NUM))
            return nullptr;

    return box_color[idx];
}

}
