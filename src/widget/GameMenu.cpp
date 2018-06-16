//
// Created by raphael on 09.04.18.
//

#include "GameMenu.h"
#include "../manager/MenuManager.h"
#include "../GlobalGameState.h"

GameMenu::GameMenu(const std::shared_ptr<NuklearContext> &ctx, Level* level) : ctx(ctx), level(level) {
    this->resize(ctx->window()->getWidth(), ctx->window()->getHeight());
}

void GameMenu::render() {
    if (!visible)
        return;

    nk_style_set_font(ctx->context(), &ctx->getFonts()[1]->handle);
    if(nk_begin(ctx->context(), "Menu", nk_rect(x, y, WIDTH, HEIGHT), NK_WINDOW_TITLE)) {
        nk_style_set_font(ctx->context(), &ctx->getFonts()[1]->handle);
        nk_layout_row_dynamic(ctx->context(), 29, 1);
        nk_label(ctx->context(), "Graphic Settings", NK_TEXT_ALIGN_CENTERED);
        nk_style_set_font(ctx->context(), &ctx->getFonts()[0]->handle);
        nk_layout_row_dynamic(ctx->context(), 25, 2);
        nk_label(ctx->context(), "Gamma: ", NK_TEXT_LEFT);
        nk_slider_float(ctx->context(), 0.0f, &this->gamma, 5.0f, 0.1f);
        nk_layout_row_dynamic(ctx->context(), 25, 2);
        nk_label(ctx->context(), "Ambient: ", NK_TEXT_LEFT);
        nk_slider_float(ctx->context(), 0.0f, &this->ambient, 1.0f, 0.05f);

        nk_layout_row_dynamic(ctx->context(), 8, 1);
        nk_style_set_font(ctx->context(), &ctx->getFonts()[1]->handle);
        nk_layout_row_dynamic(ctx->context(), 29, 1);
        nk_label(ctx->context(), "Controls", NK_TEXT_ALIGN_CENTERED);
        nk_style_set_font(ctx->context(), &ctx->getFonts()[0]->handle);
        nk_label(ctx->context(), "Mouse Speed: ", NK_TEXT_LEFT);
        nk_slider_float(ctx->context(), 0.001f, &ctx->window()->getScene()->getCamera().mouseSensitivity, 2.0f, 0.005f);

        nk_layout_row_dynamic(ctx->context(), 25, 1);
        nk_layout_row_dynamic(ctx->context(), 25, 1);
        if (nk_button_label(ctx->context(), "Resume Game")) {
            this->hide();
            MenuManager::hideMenu(true);
        }
        nk_layout_row_dynamic(ctx->context(), 25, 1);
        if (nk_button_label(ctx->context(), "Back to main Menu")) {
            GlobalGameState::state = GlobalGameState::AFTER_LEVEL;
            MenuManager::hideMenu(false);
            this->visible = false;
        }

        nk_layout_row_dynamic(ctx->context(), 16, 1);
        nk_layout_row_dynamic(ctx->context(), 25, 1);
        if (nk_button_label(ctx->context(), "Exit Game")) {
            ctx->window()->close();
        }
    }

    nk_end(ctx->context());

    this->ctx->window()->setScreenGamma(this->gamma);
    if (ambient != lastFrameAmbient) {
        lastFrameAmbient = ambient;
        ctx->window()->getScene()->getLights()[0].ambient = glm::vec3(ambient);
        ctx->window()->getScene()->makeLightsDirty();
    }
}

void GameMenu::resize(float x, float y) {
    this->x = static_cast<int>(x / 2.0f - WIDTH / 2.0f);
    this->y = static_cast<int>(y / 2.0f - 380 / 2.0f);
}

void GameMenu::show() {
    this->gamma = ctx->window()->getScreenGamma();
    this->ambient = ctx->window()->getScene()->getLights()[0].ambient.x;
    this->lastFrameAmbient = ambient;

    level->pause();
    this->visible = true;
}

void GameMenu::hide() {
    level->resume();
    this->visible = false;
}
