//
// Created by Raphael on 15.04.2018.
//

#include "PlayerHud.h"

PlayerHud::PlayerHud(std::shared_ptr<Font> font, float width, float height) :
    font(font), plus(std::string("+"), font, 0.0f, 0.0f, 1.0f, glm::vec3(0.5f, 0.5f, 0.5f)),
    hLabel(std::string("Health: "), font, 3.0f, height - 52.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f)),
    sLabel(std::string("Shield: "), font, 3.0f, height - font->getSize()+2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f)),
    hProgress(std::string(""), font, 1.0f, 1.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f)),
    sProgress(std::string(""), font, 1.0f, 1.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f)) {

    hProgress.setPosition(3.0f + hLabel.getWidth(), height - 52.0f);
    sProgress.setPosition(3.0f + sLabel.getWidth(), height - font->getSize());

    plus.setPosition(width/2-plus.getWidth()/2, height/2-font->getSize()/2);

    setHealth(100);
    setShield(100);
}


void PlayerHud::render(const glm::mat4 &projection) {
    hLabel.render(projection);
    sLabel.render(projection);
    hProgress.render(projection);
    sProgress.render(projection);
    plus.render(projection);
}

void PlayerHud::resize(float x, float y) {
    hLabel.setPosition(3.0f, y - 52.0f);
    sLabel.setPosition(3.0f, y - font->getSize());
    hProgress.setPosition(3.0f + hLabel.getWidth(), y - 52.0f);
    sProgress.setPosition(3.0f + sLabel.getWidth(), y - font->getSize()+2.0f);
    plus.setPosition(x/2-plus.getWidth()/2, y/2-font->getSize()/2);
}

void PlayerHud::setHealth(int health) {
    char buffer[24];
    int len = snprintf(buffer, 24, "[========%3d%%========]", health);

    this->hProgress.setText(std::string(buffer, buffer+len));
}

void PlayerHud::setShield(int shield) {
    char buffer[18];
    int len = snprintf(buffer, 18, "[===%3d%%===]", shield);

    this->sProgress.setText(std::string(buffer, buffer+len));
}