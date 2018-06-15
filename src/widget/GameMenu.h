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
    Level* level;
    const int HEIGHT = 380;
    const int WIDTH  = 250;

    int x,y;

public:
    explicit GameMenu(const std::shared_ptr<NuklearContext> &ctx, Level* level);

    void render() override;
    void resize(float x, float y) override;

    void show() override;
    void hide() override;

    float gamma     = 1.0f;

    // light stuff:
    float ambient   = 0.0f;
    float diffuse   = 0.0f;
    float specular  = 0.0f;
    float power     = 0.0f;

};


#endif //STAGE_FIGHTER_GAMEMENU_H
