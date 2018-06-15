//
// Created by raphael on 15.06.18.
//

#ifndef STAGE_FIGHTER_MENUMANAGER_H
#define STAGE_FIGHTER_MENUMANAGER_H

#include <chrono>
#include <memory>
#include <vector>
#include <map>

#include "../Window.h"
#include "../widget/DebugTextHud.h"
#include "../widget/NuklearCtx.h"
#include "../level/Level.h"

class MenuManager {

public:
    enum View {
        GAME_MENU, HELP_MENU, MAIN_MENU, NONE
    };

private:
    static Window *window;
    static std::shared_ptr<DebugTextHud> debugTextHud;
    static Level *level;

    static std::shared_ptr<NuklearContext> nkContext;
    static std::map<MenuManager::View, std::shared_ptr<NuklearWidget>> menus;

    static MenuManager::View currentVisibleMenu;
public:

    static void init(Window *window);
    static void destroy();

    static void showMenu(MenuManager::View view, bool showCursor = true);
    static void hideMenu(bool hideCursor = true);
    static void toggleMenu(MenuManager::View view, bool showCursor = true);


    static Window *getWindow() {
        return MenuManager::window;
    }

    static MenuManager::View  getCurrentVisibleMenu() {
        return MenuManager::currentVisibleMenu;
    }

    static std::shared_ptr<NuklearContext> getNuklearContext() {
        return MenuManager::nkContext;
    }

    static std::map<MenuManager::View, std::shared_ptr<NuklearWidget>> &getMenus() {
        return MenuManager::menus;
    };

    static void transitionIntoLevel(Level *);

    static void update(std::chrono::duration<double, std::milli> delta);

};


#endif //STAGE_FIGHTER_MENUMANAGER_H
