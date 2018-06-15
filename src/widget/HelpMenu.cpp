//
// Created by Raphael on 29.04.2018.
//

#include "HelpMenu.h"

void HelpMenu::render() {
    if(!visible)
        return;

    nk_style_set_font(ctx->context(), &ctx->getFonts()[1]->handle);
    if(nk_begin(ctx->context(), "Help", nk_rect(x, y, WIDTH, HEIGHT), this->flags)) {

        nk_style_set_font(ctx->context(), &ctx->getFonts()[0]->handle);
        nk_layout_row_dynamic(ctx->context(), 12, 1);
        nk_label(ctx->context(), "--- Movements ---", NK_TEXT_ALIGN_CENTERED);
        nk_layout_row_dynamic(ctx->context(), 25, 2);
        nk_label(ctx->context(), "Move forward", NK_TEXT_LEFT);
        nk_label(ctx->context(), "[ W ]", NK_TEXT_CENTERED);
        nk_label(ctx->context(), "Move backward", NK_TEXT_LEFT);
        nk_label(ctx->context(), "[ S ]", NK_TEXT_CENTERED);
        nk_label(ctx->context(), "Move right", NK_TEXT_LEFT);
        nk_label(ctx->context(), "[ D ]", NK_TEXT_CENTERED);
        nk_label(ctx->context(), "Move left", NK_TEXT_LEFT);
        nk_label(ctx->context(), "[ A ]", NK_TEXT_CENTERED);
        nk_label(ctx->context(), "Jump", NK_TEXT_LEFT);
        nk_label(ctx->context(), "[ Spacebar ]", NK_TEXT_CENTERED);
        nk_label(ctx->context(), "Hit Enemies", NK_TEXT_LEFT);
        nk_label(ctx->context(), "[ Mouse Left ]", NK_TEXT_CENTERED);

        nk_layout_row_dynamic(ctx->context(), 25, 1);
        nk_layout_row_dynamic(ctx->context(), 12, 1);
        nk_label(ctx->context(), "--- Menus ---", NK_TEXT_ALIGN_CENTERED);

        nk_layout_row_dynamic(ctx->context(), 25, 2);
        nk_label(ctx->context(), "Game Menu", NK_TEXT_LEFT);
        nk_label(ctx->context(), "[ ESC ]", NK_TEXT_CENTERED);
        nk_label(ctx->context(), "Help Menu", NK_TEXT_LEFT);
        nk_label(ctx->context(), "[ F1 ]", NK_TEXT_CENTERED);
        nk_label(ctx->context(), "FPS Counter", NK_TEXT_LEFT);
        nk_label(ctx->context(), "[ F2 ]", NK_TEXT_CENTERED);
    }

    nk_end(ctx->context());
}

void HelpMenu::resize(float x, float y) {
    this->x = static_cast<int>(x / 2.0f - WIDTH / 2.0f);
    this->y = static_cast<int>(y / 2.0f - HEIGHT / 2.0f);
}

void HelpMenu::show() {
//    this->ctx->hideAll();

    visible = true;
    ctx->window()->showCursor();
}

void HelpMenu::hide() {
    visible = false;
    ctx->window()->hideCursor();
}

bool HelpMenu::toggle() {
    if (!visible) { show(); return true; }
    else { hide(); return false; }
}

HelpMenu::HelpMenu(const std::shared_ptr<NuklearContext> &ctx) : ctx(ctx) {
    this->resize(ctx->window()->getWidth(), ctx->window()->getHeight());
}
