//
// Created by Raphael on 15.06.2018.
//

#ifndef STAGE_FIGHTER_SETTINGSMENU_H
#define STAGE_FIGHTER_SETTINGSMENU_H


#include "NuklearCtx.h"

class SettingsMenu : public NuklearWidget {
private:
    const std::shared_ptr<NuklearContext> ctx;
    const int HEIGHT = 340;
    const int WIDTH  = 250;

    int x,y;

    float screenGamma;
    float globalAudioVolume = 1.0f;
public:
    explicit SettingsMenu(const std::shared_ptr<NuklearContext> &ctx);

    void render() override;
    void resize(float x, float y) override;
    void show() override;
    void hide() override;

};


#endif //STAGE_FIGHTER_SETTINGSMENU_H
