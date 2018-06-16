//
// Created by Raphael on 15.06.2018.
//

#ifndef STAGE_FIGHTER_LAUNCHSCREEN_H
#define STAGE_FIGHTER_LAUNCHSCREEN_H


#include "widget/Image.h"
#include "widget/Label.h"
#include "widget/NuklearCtx.h"

class LaunchScreen : virtual public Widget, virtual public NuklearWidget {

private:
    std::shared_ptr<NuklearContext> ctx;
    std::shared_ptr<LaunchScreen> not_so_smart_pointer;

    Label header;
    Image background;

    const int HEIGHT = 380;
    const int WIDTH  = 250;
    int x=0,y=0;

    bool visible = false;

public:
    explicit LaunchScreen(Window *);
    void resize(float x, float y) override;

    void show() override;
    void hide() override;

    void render(const glm::mat4 &projection, float screenGamma) override;
    void render() override;

    bool isVisible() { return this->visible; }
};


#endif //STAGE_FIGHTER_LAUNCHSCREEN_H
