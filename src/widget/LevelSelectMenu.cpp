//
// Created by Raphael on 15.06.2018.
//

#include <memory>
#include <string>

#include "LevelSelectMenu.h"
#include "../GlobalGameState.h"
#include "../manager/MenuManager.h"

LevelSelectMenu::LevelSelectMenu(const std::shared_ptr<NuklearContext> &ctx)
        : ctx(ctx) {
    state.dofile(std::string("../resources/level/index.lua"));
    this->resize(ctx->window()->getWidth(), ctx->window()->getHeight());
}

void LevelSelectMenu::render() {
    std::string selected;

    nk_style_set_font(ctx->context(), &ctx->getFonts()[1]->handle);
    if(nk_begin(ctx->context(), "Select a Level:", nk_rect(x, y, WIDTH, HEIGHT), NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {
        nk_style_set_font(ctx->context(), &ctx->getFonts()[0]->handle);

        for (auto &level : state["levels"].get<std::map<std::string, kaguya::LuaTable>>()) {
            nk_layout_row_dynamic(ctx->context(), 25, 1);
            if (nk_button_label(ctx->context(), level.first.c_str())) {
                selected = level.second["file"].get<std::string>();
            }
        }

    }
    nk_end(ctx->context());

    if (!selected.empty()) {
        MenuManager::hideMenu(true);
        GlobalGameState::level = std::make_shared<Level>(std::string("../resources/level/") + selected);
        GlobalGameState::state = GlobalGameState::LEVEL_LOADING_FINISHED;
    }
}

void LevelSelectMenu::resize(float x, float y) {
    this->x = static_cast<int>(x / 2.0f - WIDTH / 2.0f);
    this->y = static_cast<int>(y / 2.0f - HEIGHT / 2.0f);
}

void LevelSelectMenu::show() {}
void LevelSelectMenu::hide() {}
