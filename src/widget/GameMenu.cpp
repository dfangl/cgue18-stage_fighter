//
// Created by raphael on 09.04.18.
//

#include "GameMenu.h"

GameMenu::GameMenu(const std::shared_ptr<NuklearContext> &ctx) : ctx(ctx) {
    this->resize(ctx->window()->getWidth(), ctx->window()->getHeight());
}

void GameMenu::render() {
    if (!visible)
        return;

    if(nk_begin(ctx->context(), "Menu", nk_rect(x, y, WIDTH, HEIGHT), NK_WINDOW_TITLE)) {
        nk_layout_row_dynamic(ctx->context(), 25, 1);
        if (nk_button_label(ctx->context(), "Exit Game")) {
            ctx->window()->close();
        }
    }

    nk_end(ctx->context());
}

void GameMenu::resize(float x, float y) {
    this->x = static_cast<int>(x / 2.0f - WIDTH / 2.0f);
    this->y = static_cast<int>(y / 2.0f - HEIGHT / 2.0f);
}

void GameMenu::show() {
    visible = true;
    ctx->window()->showCurosor();
}

void GameMenu::hide() {
    visible = false;
    ctx->window()->hideCursor();
}
