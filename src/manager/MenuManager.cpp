//
// Created by raphael on 15.06.18.
//

#include "MenuManager.h"
#include "../widget/HelpMenu.h"

Window* MenuManager::window = nullptr;
Level* MenuManager::level = nullptr;
std::shared_ptr<DebugTextHud> MenuManager::debugTextHud;
std::shared_ptr<NuklearContext> MenuManager::nkContext;
std::map<MenuManager::View, std::shared_ptr<NuklearWidget>> MenuManager::menus;
MenuManager::View MenuManager::currentVisibleMenu = MenuManager::View::NONE;

void MenuManager::init(Window *window) {
    // Init Environment:
    MenuManager::window = window;
    MenuManager::nkContext = std::make_shared<NuklearContext>(window);
    MenuManager::debugTextHud = std::make_shared<DebugTextHud>(window->getScene());
    MenuManager::menus[MenuManager::View::HELP_MENU] = std::make_shared<HelpMenu>(MenuManager::nkContext);

    // Add Debug stuff to Window:
    window->addWidget(debugTextHud);
    window->registerKeyCallback(debugTextHud->getKeyCallback());
    window->registerKeyCallback([](int key, int UNUSED(scancode), int action, int UNUSED(mods)){
        if (key == GLFW_KEY_F1 && action == GLFW_RELEASE) {
            if (currentVisibleMenu == View::NONE) { if (level != nullptr) level->pause(); showMenu(View::HELP_MENU); }
            else { hideMenu(); if (level != nullptr) level->resume(); }
        }
    });

    window->addWidget(nkContext);
}

void MenuManager::destroy() {
    MenuManager::window = nullptr;
    MenuManager::level  = nullptr;
    MenuManager::nkContext.reset();
    MenuManager::menus.clear();
    MenuManager::debugTextHud.reset();
}

void MenuManager::hideMenu(bool hideCursor) {
    menus[currentVisibleMenu]->hide();
    currentVisibleMenu = View::NONE;

    MenuManager::nkContext->clear();
    MenuManager::nkContext->enabled = false;

    if (hideCursor)
        MenuManager::window->hideCursor();
}

void MenuManager::showMenu(MenuManager::View view, bool showCursor) {
    currentVisibleMenu = view;

    MenuManager::nkContext->add(menus[view]);
    MenuManager::nkContext->enabled = true;
    menus[view]->show();

    if (showCursor)
        MenuManager::window->showCursor();
}

void MenuManager::transitionIntoLevel(Level *l) {
    MenuManager::level = l;
}

void MenuManager::toggleMenu(MenuManager::View view, bool showCursor) {
    if (currentVisibleMenu == view) hideMenu(showCursor);
    else showMenu(view, showCursor);
}

void MenuManager::update(std::chrono::duration<double, std::milli> delta) {
    debugTextHud->update(delta);
    nkContext->newFrame();
}
