//
// Created by raphael on 09.04.18.
//

#include "GameMenu.h"

GameMenu::GameMenu(const std::shared_ptr<NuklearContext> &ctx, const std::shared_ptr<Level> &level) : ctx(ctx), level(level) {
    this->resize(ctx->window()->getWidth(), ctx->window()->getHeight());
}

void GameMenu::render() {
    if (!visible)
        return;

    if(nk_begin(ctx->context(), "Menu", nk_rect(x, y, WIDTH, HEIGHT), NK_WINDOW_TITLE)) {
        nk_layout_row_dynamic(ctx->context(), 12, 1);
        nk_label(ctx->context(), "--- Light ---", NK_TEXT_ALIGN_CENTERED);
        nk_layout_row_dynamic(ctx->context(), 25, 2);
        nk_label(ctx->context(), "Ambient: ", NK_TEXT_LEFT);
        nk_slider_float(ctx->context(), 0.0f, &this->ambient, 1.0f, 0.05f);
        nk_label(ctx->context(), "Diffuse: ", NK_TEXT_LEFT);
        nk_slider_float(ctx->context(), 0.0f, &this->diffuse, 1.0f, 0.05f);
        nk_label(ctx->context(), "Specular: ", NK_TEXT_LEFT);
        nk_slider_float(ctx->context(), 0.0f, &this->specular, 1.0f, 0.05f);
        nk_label(ctx->context(), "Power: ", NK_TEXT_LEFT);
        nk_slider_float(ctx->context(), 0.0f, &this->power, 100.0f, 0.1f);
        nk_layout_row_dynamic(ctx->context(), 25, 1);

        nk_layout_row_dynamic(ctx->context(), 12, 1);
        nk_label(ctx->context(), "--- Settings ---", NK_TEXT_ALIGN_CENTERED);
        nk_layout_row_dynamic(ctx->context(), 25, 2);
        nk_label(ctx->context(), "Gamma: ", NK_TEXT_LEFT);
        nk_slider_float(ctx->context(), 0.0f, &this->gamma, 5.0f, 0.1f);

        nk_layout_row_dynamic(ctx->context(), 25, 1);
        nk_layout_row_dynamic(ctx->context(), 25, 1);
        if (nk_button_label(ctx->context(), "Exit Game")) {
            ctx->window()->close();
        }
        nk_layout_row_dynamic(ctx->context(), 25, 1);
        nk_layout_row_dynamic(ctx->context(), 25, 1);
        if (nk_button_label(ctx->context(), "Resume game")) {
            ctx->enabled = false;
            this->hide();
            level->resume();
        }
    }

    nk_end(ctx->context());

    this->ctx->window()->setScreenGamma(this->gamma);
    auto &l = this->ctx->window()->getScene()->getLights()[0];
    l->ambient = glm::vec3(ambient);
    l->specular = glm::vec3(specular);
    l->diffuse = glm::vec3(diffuse);
    l->power = power;
}

void GameMenu::resize(float x, float y) {
    this->x = static_cast<int>(x / 2.0f - WIDTH / 2.0f);
    this->y = static_cast<int>(y / 2.0f - HEIGHT / 2.0f);
}

void GameMenu::show() {
    auto &l = this->ctx->window()->getScene()->getLights()[0];
    ambient = l->ambient.x;
    specular = l->specular.x;
    diffuse = l->diffuse.x;
    power = l->power;

    this->ctx->hideAll();

    visible = true;
    ctx->window()->showCursor();
}

void GameMenu::hide() {
    visible = false;
    ctx->window()->hideCursor();
}
