//
// Created by Raphael on 16.06.2018.
//

#ifndef STAGE_FIGHTER_BACKTOMENUWINDOW_H
#define STAGE_FIGHTER_BACKTOMENUWINDOW_H


#include "NuklearCtx.h"

class BackToMenuWindow : public NuklearWidget {
private:
    const std::shared_ptr<NuklearContext> ctx;
    const int HEIGHT = 122;
    const int WIDTH  = 275;

    int x,y;
public:
    BackToMenuWindow(const std::shared_ptr<NuklearContext> &ctx);

    void render() override;
    void resize(float x, float y) override;

    void show() override {};
    void hide() override {};
};


#endif //STAGE_FIGHTER_BACKTOMENUWINDOW_H
