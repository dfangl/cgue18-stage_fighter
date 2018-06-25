//
// Created by Raphael on 15.06.2018.
//

#include "LaunchScreen.h"
#include "manager/FontManager.h"
#include "manager/MenuManager.h"
#include "widget/LevelSelectMenu.h"
#include "widget/SettingsMenu.h"


LaunchScreen::LaunchScreen(Window *window, irrklang::ISound *sound)
        : ctx(std::make_shared<NuklearContext>(window)),
          header(std::string("Stage Fighter"), FontManager::get("Metamorphous-72") , 1.0f, 1.0f, 1.0f, glm::vec3(0.09f, 0.09f, 0.09f)),
          background(0,0,window->getWidth(), window->getHeight(),TextureManager::load("__gen_marble")) {

    this->not_so_smart_pointer = std::shared_ptr<LaunchScreen>(this);

    ctx->context()->style.window.fixed_background = nk_style_item_color(nk_rgba(15, 15, 15, 0));
    ctx->context()->style.button.normal = nk_style_item_color(nk_rgba(15, 15, 15, 145));
    ctx->context()->style.button.border_color = nk_rgb(0, 0, 0);
    ctx->context()->style.button.border = 0.2f;
    ctx->context()->style.button.text_normal = nk_rgb(245, 245, 245);
    ctx->context()->style.button.text_hover = nk_rgb(245, 245, 245);

    this->backgroundMusic = sound;
    this->backgroundMusic->setVolume(0.20f);
    this->resize(window->getWidth(), window->getHeight());
}


void LaunchScreen::render() {
    int state = 0;

    nk_style_set_font(ctx->context(), &ctx->getFonts()[1]->handle);
    if(nk_begin(ctx->context(), "__GAME__MENU__HEADER__", nk_rect(x, y, WIDTH, HEIGHT), NK_WINDOW_BORDER)) {
        nk_style_set_font(ctx->context(), &ctx->getFonts()[1]->handle);
        nk_layout_row_dynamic(ctx->context(), 41, 1);
        if (nk_button_label(ctx->context(), "Play")) {
            AudioManager::audioEngine->play2D("../resources/audio/button-50.mp3");
            state = 1;
        }
        nk_layout_row_dynamic(ctx->context(), 41, 1);
        if (nk_button_label(ctx->context(), "Settings")) {
            AudioManager::audioEngine->play2D("../resources/audio/button-50.mp3");
            state = 2;
        }
        nk_layout_row_dynamic(ctx->context(), 175, 1);
        nk_layout_row_dynamic(ctx->context(), 41, 1);
        if (nk_button_label(ctx->context(), "Exit Game")) {
            AudioManager::audioEngine->play2D("../resources/audio/button-50.mp3");
            ctx->window()->close();
        }
    }
    nk_end(ctx->context());


    switch (state) {
        case 1:
            MenuManager::getMenus()[MenuManager::LEVEL_SELECT] = std::make_shared<LevelSelectMenu>(MenuManager::getNuklearContext());
            MenuManager::showMenu(MenuManager::LEVEL_SELECT);
            break;
        case 2:
            MenuManager::getMenus()[MenuManager::SETTINGS_MENU] = std::make_shared<SettingsMenu>(MenuManager::getNuklearContext());
            MenuManager::showMenu(MenuManager::SETTINGS_MENU);
            break;
        default: /* dafuq did you do? */ break;
    }
}

void LaunchScreen::resize(float x, float y) {
    header.setPosition(x/2.0f - header.getWidth()/2.0f, 128.0f);

    this->x = static_cast<int>(x / 2.0f - WIDTH / 2.0f);
    this->y = static_cast<int>(y / 2.0f - HEIGHT / 2.0f);
}

void LaunchScreen::show() {
    this->visible = true;
    ctx->window()->processCameraMouseMovement(false);
    ctx->window()->processCameraKeyMovement(false);
    ctx->window()->showCursor();

    ctx->window()->addWidget(not_so_smart_pointer);
    ctx->window()->addWidget(ctx);

    // Move Menu stuff back so Background image does not overdraw it
    ctx->window()->removeWidget(MenuManager::getNuklearContext());
    ctx->window()->addWidget(MenuManager::getNuklearContext());

    this->backgroundMusic->setPlayPosition(0);
    this->backgroundMusic->setIsPaused(false);
}
void LaunchScreen::hide() {
    this->visible = false;
    ctx->window()->removeWidget(not_so_smart_pointer);
    ctx->window()->removeWidget(ctx);


    this->backgroundMusic->setIsPaused(true);
}

void LaunchScreen::render(const glm::mat4 &projection, float screenGamma) {
    this->background.render(projection, screenGamma);
    this->header.render(projection, screenGamma);

    if (MenuManager::getCurrentVisibleMenu() == MenuManager::View::NONE) {
        render();
        ctx->newFrame();
        ctx->render(projection, screenGamma);
    }

}
