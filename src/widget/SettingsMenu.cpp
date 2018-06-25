//
// Created by Raphael on 15.06.2018.
//

#include "SettingsMenu.h"
#include "../GlobalGameState.h"
#include "../manager/MenuManager.h"

SettingsMenu::SettingsMenu(const std::shared_ptr<NuklearContext> &ctx) : ctx(ctx){
    this->screenGamma = ctx->window()->getScreenGamma();
    this->resize(ctx->window()->getWidth(), ctx->window()->getHeight());
}

void SettingsMenu::render() {
    bool menuExitAction = false;

    nk_style_set_font(ctx->context(), &ctx->getFonts()[1]->handle);
    if(nk_begin(ctx->context(), "Settings", nk_rect(x, y, WIDTH, HEIGHT), NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
        nk_style_set_font(ctx->context(), &ctx->getFonts()[1]->handle);
        nk_layout_row_dynamic(ctx->context(), 26, 1);
        nk_label(ctx->context(), "Graphics", NK_TEXT_ALIGN_CENTERED);

        nk_style_set_font(ctx->context(), &ctx->getFonts()[0]->handle);
        nk_layout_row_dynamic(ctx->context(), 28, 2);
        nk_label(ctx->context(), "Gamma: ", NK_TEXT_LEFT);
        nk_slider_float(ctx->context(), 0.0f, &screenGamma, 5.0f, 0.1f);
        nk_layout_row_dynamic(ctx->context(), 25, 1);

        nk_style_set_font(ctx->context(), &ctx->getFonts()[1]->handle);
        nk_layout_row_dynamic(ctx->context(), 29, 1);
        nk_label(ctx->context(), "Controls", NK_TEXT_ALIGN_CENTERED);
        nk_style_set_font(ctx->context(), &ctx->getFonts()[0]->handle);
        nk_label(ctx->context(), "Mouse Speed: ", NK_TEXT_LEFT);
        nk_slider_float(ctx->context(), 0.001f, &ctx->window()->getScene()->getCamera().mouseSensitivity, 2.0f, 0.005f);

        if (nk_button_label(ctx->context(), "Back")) {
            AudioManager::audioEngine->play2D("../resources/audio/button-50.mp3");
           menuExitAction = true;
        }
    }
    nk_end(ctx->context());

    ctx->window()->setScreenGamma(this->screenGamma);
    if (menuExitAction) {
        GlobalGameState::state = GlobalGameState::IN_MENU;
        MenuManager::hideMenu(false);
    }
}

void SettingsMenu::resize(float x, float y) {
    this->x = static_cast<int>(x / 2.0f - WIDTH / 2.0f);
    this->y = static_cast<int>(y / 2.0f - HEIGHT / 2.0f);
}

void SettingsMenu::show() {}
void SettingsMenu::hide() {}
