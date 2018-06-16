//
// Created by Raphael on 16.06.2018.
//

#include "BackToMenuWindow.h"
#include "../GlobalGameState.h"
#include "../manager/MenuManager.h"

BackToMenuWindow::BackToMenuWindow(const std::shared_ptr<NuklearContext> &ctx) : ctx(ctx) {
    this->resize(ctx->window()->getWidth(), ctx->window()->getHeight());
}

void BackToMenuWindow::render() {
    ctx->context()->style.window.fixed_background = nk_style_item_color(nk_rgba(15, 15, 15, 0));
    ctx->context()->style.button.normal = nk_style_item_color(nk_rgba(15, 15, 15, 200));
    ctx->context()->style.button.border_color = nk_rgb(0, 0, 0);
    ctx->context()->style.button.border = 0.2f;
    ctx->context()->style.button.text_normal = nk_rgb(245, 245, 245);
    ctx->context()->style.button.text_hover = nk_rgb(245, 245, 245);

    nk_style_set_font(ctx->context(), &ctx->getFonts()[1]->handle);
    if(nk_begin(ctx->context(), "__LEVEL_FINISHED_WINDOW__", nk_rect(x, y, WIDTH, HEIGHT), NK_WINDOW_BORDER)) {
        nk_style_set_font(ctx->context(), &ctx->getFonts()[1]->handle);
        nk_layout_row_dynamic(ctx->context(), 32, 1);
        if (nk_button_label(ctx->context(), "Play again")) {
            GlobalGameState::state = GlobalGameState::LEVEL_PLAY_AGAIN;
            MenuManager::hideMenu(false);
        }
        nk_layout_row_dynamic(ctx->context(), 8, 1);
        nk_layout_row_dynamic(ctx->context(), 32, 1);
        if (nk_button_label(ctx->context(), "Back to main Menu")) {
            GlobalGameState::state = GlobalGameState::AFTER_LEVEL;
            MenuManager::hideMenu(false);
        }
    }
    nk_end(ctx->context());
    ctx->resetStyle();
}

void BackToMenuWindow::resize(float x, float y) {
    this->x = static_cast<int>(x / 2.0f - WIDTH / 2.0f);
    this->y = static_cast<int>(y / 2.0f - 320 / 2.0f);
}
