//
// Created by raphael on 09.04.18.
//

#ifndef STAGE_FIGHTER_GAMEMENU_H
#define STAGE_FIGHTER_GAMEMENU_H

#include <memory>

#include "NuklearCtx.h"

class GameMenu : public NuklearWidget {

private:
    const std::shared_ptr<NuklearContext> ctx;
    const int HEIGHT = 250;
    const int WIDTH  = 250;

    int x,y;
    bool visible = false;

public:
    explicit GameMenu(const std::shared_ptr<NuklearContext> &ctx);

    void render() override;
    void resize(float x, float y) override;

    void show();
    void hide();

};


#endif //STAGE_FIGHTER_GAMEMENU_H
