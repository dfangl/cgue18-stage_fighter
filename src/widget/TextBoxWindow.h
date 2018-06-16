//
// Created by Raphael on 16.06.2018.
//

#ifndef STAGE_FIGHTER_TEXTBOXWINDOW_H
#define STAGE_FIGHTER_TEXTBOXWINDOW_H


#include "NuklearCtx.h"

class TextBoxWindow : public NuklearWidget {
private:
    const std::shared_ptr<NuklearContext> ctx;
    const int HEIGHT = 122;
    const int WIDTH  = 500;

    int x,y;
    std::vector<std::string> textlines;

public:
    TextBoxWindow(const std::shared_ptr<NuklearContext> &ctx);

    void setText(const std::string &text);

    void render() override;
    void resize(float x, float y) override;

    void show() override;
    void hide() override;
};


#endif //STAGE_FIGHTER_TEXTBOXWINDOW_H
