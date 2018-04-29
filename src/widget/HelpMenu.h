//
// Created by Raphael on 29.04.2018.
//

#ifndef STAGE_FIGHTER_HELPMENU_H
#define STAGE_FIGHTER_HELPMENU_H


#include "NuklearCtx.h"

class HelpMenu : public NuklearWidget {
private:
    const std::shared_ptr<NuklearContext> ctx;
    const int HEIGHT = 425;
    const int WIDTH  = 280;

    int x,y;
    nk_flags flags = NK_WINDOW_TITLE;

public:
    explicit HelpMenu(const std::shared_ptr<NuklearContext> &ctx);

    void render() override;
    void resize(float x, float y) override;

    void show() override;
    void hide() override;
    bool toggle();

};


#endif //STAGE_FIGHTER_HELPMENU_H
