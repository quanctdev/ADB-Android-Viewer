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

#include "../../ADBViewer.h"
#include  "../gui/guiPopUpMenu.WINAPI.h"

int32_t AppBrowserList::popupmenu_apk()
    {
        int32_t idx       = -1;
        HMENU  l_hPopMenu = nullptr;

        do
        {
            if (!(l_hPopMenu = ::CreatePopupMenu()))
                break;

            MENU_ITEM_ADD(ID_CMD_POP_MENU264, ResManager::IndexStringPopUpMenu::RES_STR_POPUP_58, l_hPopMenu);
            ::AppendMenuW(l_hPopMenu,  MF_SEPARATOR, 0, nullptr);
            MENU_ITEM_ADD(ID_CMD_POP_MENU263, ResManager::IndexStringPopUpMenu::RES_STR_POPUP_57, l_hPopMenu);

            guiPopUp::style(l_hPopMenu);
            idx = guiPopUp::show(l_hPopMenu, guiBase::GetGui<SDL_Window>());
        }
        while (0);

        if (l_hPopMenu)
            ::DestroyMenu(l_hPopMenu);

        return idx;
    }

int32_t AppBrowserList::popupmenu_info()
    {
        int32_t idx       = -1;
        HMENU  l_hPopMenu = nullptr;

        do
        {
            if (!(l_hPopMenu = ::CreatePopupMenu()))
                break;

            MENU_ITEM_ADD(ID_CMD_POP_MENU263, ResManager::IndexStringPopUpMenu::RES_STR_POPUP_57, l_hPopMenu);
            //::AppendMenuW(l_hPopMenu,  MF_SEPARATOR, 0, nullptr);

            guiPopUp::style(l_hPopMenu);
            idx = guiPopUp::show(l_hPopMenu, guiBase::GetGui<SDL_Window>());
        }
        while (0);

        if (l_hPopMenu)
            ::DestroyMenu(l_hPopMenu);

        return idx;
    }

int32_t AppBrowserList::popupmenu_file(bool isfattr)
    {
        int32_t idx       = -1;
        HMENU  l_hPopMenu = nullptr;
        HMENU  l_hPermMenu = nullptr;

        do
        {
            if (!(l_hPopMenu = ::CreatePopupMenu()))
                break;

            MENU_ITEM_ADD(ID_CMD_POP_MENU261, ResManager::IndexStringPopUpMenu::RES_STR_POPUP_55, l_hPopMenu);
            MENU_ITEM_ADD(ID_CMD_POP_MENU260, ResManager::IndexStringPopUpMenu::RES_STR_POPUP_54, l_hPopMenu);
            MENU_ITEM_ADD(ID_CMD_POP_MENU262, ResManager::IndexStringPopUpMenu::RES_STR_POPUP_56, l_hPopMenu);
            if (isfattr)
            {
                if (!(l_hPermMenu = ::CreateMenu()))
                    break;

                MENU_ITEM_ADD(ID_CMD_POP_MENU265, ResManager::IndexStringPopUpMenu::RES_STR_POPUP_60, l_hPermMenu);
                MENU_ITEM_ADD(ID_CMD_POP_MENU266, ResManager::IndexStringPopUpMenu::RES_STR_POPUP_61, l_hPermMenu);
                MENU_ITEM_ADD(ID_CMD_POP_MENU267, ResManager::IndexStringPopUpMenu::RES_STR_POPUP_62, l_hPermMenu);
                MENU_ITEM_ADD(ID_CMD_POP_MENU268, ResManager::IndexStringPopUpMenu::RES_STR_POPUP_63, l_hPermMenu);
                MENU_ITEM_ADD(ID_CMD_POP_MENU269, ResManager::IndexStringPopUpMenu::RES_STR_POPUP_64, l_hPermMenu);
                MENU_ITEM_ADD(ID_CMD_POP_MENU270, ResManager::IndexStringPopUpMenu::RES_STR_POPUP_65, l_hPermMenu);

                guiPopUp::style(l_hPermMenu, MNS_AUTODISMISS | MNS_CHECKORBMP);
                ::AppendMenuW(l_hPopMenu,  MF_SEPARATOR, 0, nullptr);
                MENU_ADD(l_hPermMenu, ResManager::IndexStringPopUpMenu::RES_STR_POPUP_59, l_hPopMenu);
            }
            ::AppendMenuW(l_hPopMenu,  MF_SEPARATOR, 0, nullptr);
            MENU_ITEM_ADD(ID_CMD_POP_MENU263, ResManager::IndexStringPopUpMenu::RES_STR_POPUP_57, l_hPopMenu);

            guiPopUp::style(l_hPopMenu);
            idx = guiPopUp::show(l_hPopMenu, guiBase::GetGui<SDL_Window>());
        }
        while (0);

        if (l_hPermMenu)
            ::DestroyMenu(l_hPermMenu);
        if (l_hPopMenu)
            ::DestroyMenu(l_hPopMenu);

        return idx;
    }
