//
// Created by Raphael on 15.04.2018.
//

#ifndef STAGE_FIGHTER_PLAYERHUD_H
#define STAGE_FIGHTER_PLAYERHUD_H

#include <string>
#include <memory>

#include "Widget.h"
#include "Label.h"

class PlayerHud : public Widget {

private:
    std::shared_ptr<Font> font;

    Label hLabel;
    Label sLabel;

    //Progressbar or so?
    Label hProgress;
    Label sProgress;

    Label plus;

public:
    PlayerHud(std::shared_ptr<Font> font, float width, float height);

    void render(const glm::mat4 &projection) override;
    void resize(float x, float y) override;

    void setHealth(int health);
    void setShield(int shield);

};


#endif //STAGE_FIGHTER_PLAYERHUD_H
