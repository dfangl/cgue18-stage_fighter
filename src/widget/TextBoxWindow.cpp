//
// Created by Raphael on 16.06.2018.
//

#include "TextBoxWindow.h"

#include <string>
#include <sstream>

TextBoxWindow::TextBoxWindow(const std::shared_ptr<NuklearContext> &ctx) : ctx(ctx) {
    this->resize(ctx->window()->getWidth(), ctx->window()->getHeight());
}

void TextBoxWindow::render() {
    nk_style_set_font(ctx->context(), &ctx->getFonts()[0]->handle);
    if(nk_begin(ctx->context(), "__LEVEL_TEXT_BOX__", nk_rect(x, y, WIDTH, HEIGHT), NK_WINDOW_BORDER)) {
        for (auto &text : this->textlines) {
            nk_layout_row_dynamic(ctx->context(), 15, 1);
            nk_label(ctx->context(), text.c_str(), NK_TEXT_LEFT);
        }
    }
    nk_end(ctx->context());
}

void TextBoxWindow::resize(float x, float y) {
    this->x = static_cast<int>(x / 2.0f - WIDTH / 2.0f);
    this->y = static_cast<int>(y - HEIGHT - 75);
}

void TextBoxWindow::show() {
    ctx->disableInput = true;
}

void TextBoxWindow::hide() {
    ctx->disableInput = false;
}

void TextBoxWindow::setText(const std::string &text) {
    const char delimiter = '\n';
    textlines.clear();

    std::string token;
    std::istringstream tokenStream(text);
    while (std::getline(tokenStream, token, delimiter)) {
        textlines.push_back(token);
    }
}
