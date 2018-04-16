//
// Created by Raphael on 15.04.2018.
//

#ifndef STAGE_FIGHTER_PLAYERHUD_H
#define STAGE_FIGHTER_PLAYERHUD_H

#include <glm/glm.hpp>
#include <string>
#include <memory>

#include "Widget.h"
#include "Label.h"

class PlayerHud : public Widget {

private:
    std::shared_ptr<Font> font;

    Label hLabel;
    Label sLabel;
    Label eNameLabel;

    //Progressbar or so?
    Label hProgress;
    Label sProgress;
    Label eHealth;

    Label plus;

    int health,shield;
    float height, width;

public:
    PlayerHud(std::shared_ptr<Font> font, float width, float height);

    void render(const glm::mat4 &projection) override;
    void resize(float x, float y) override;

    void setHealth(int health);
    void setShield(int shield);
    void setEnemy(std::string &name, int health, int maxHealth);

    bool showEnemy = false;

};


#endif //STAGE_FIGHTER_PLAYERHUD_H
