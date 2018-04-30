//
// Created by raphael on 09.04.18.
//

#ifndef STAGE_FIGHTER_GAMEMENU_H
#define STAGE_FIGHTER_GAMEMENU_H

#include <memory>

#include "NuklearCtx.h"
#include "../level/Level.h"

class GameMenu : public NuklearWidget {

private:
    const std::shared_ptr<NuklearContext> ctx;
    const std::shared_ptr<Level> level;
    const int HEIGHT = 380;
    const int WIDTH  = 250;

    int x,y;

public:
    explicit GameMenu(const std::shared_ptr<NuklearContext> &ctx, const std::shared_ptr<Level> &level);

    void render() override;
    void resize(float x, float y) override;

    void show() override;
    void hide() override;

    float gamma;

    // light stuff:
    float ambient;
    float diffuse;
    float specular;
    float power;

};


#endif //STAGE_FIGHTER_GAMEMENU_H
