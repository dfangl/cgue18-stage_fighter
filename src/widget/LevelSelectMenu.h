//
// Created by Raphael on 15.06.2018.
//

#ifndef STAGE_FIGHTER_LEVELSELECTMENU_H
#define STAGE_FIGHTER_LEVELSELECTMENU_H

#include <functional>

#include <kaguya/state.hpp>
#include "NuklearCtx.h"

class LevelSelectMenu : public NuklearWidget {
private:
    const std::shared_ptr<NuklearContext> ctx;
    const int HEIGHT = 340;
    const int WIDTH  = 250;

    int x,y;

    kaguya::State state;
public:
    LevelSelectMenu(const std::shared_ptr<NuklearContext> &ctx);

    void render() override;

    void resize(float x, float y) override;

    void show() override;

    void hide() override;
};


#endif //STAGE_FIGHTER_LEVELSELECTMENU_H
