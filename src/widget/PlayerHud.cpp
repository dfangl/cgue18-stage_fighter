//
// Created by Raphael on 15.04.2018.
//

#include "PlayerHud.h"
#include "../manager/ShaderManager.h"
#include "../manager/TextureManager.h"

PlayerHud::PlayerHud(std::shared_ptr<Font> font, float width, float height) :
    font(font), hudShader(ShaderManager::load("hud")),
    hLabel(std::string("Health: "), font, 3.0f, height - 52.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f)),
    sLabel(std::string("Shield: "), font, 3.0f, height - font->getSize()+2.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f)),
    hProgress(std::string(""), font, 1.0f, 1.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f)),
    sProgress(std::string(""), font, 1.0f, 1.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f)),
    eNameLabel(std::string(""), font, 1.0f, 1.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f)),
    eHealth(std::string(""), font, 1.0f, 1.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f)),
    height(height), width(width), texture(TextureManager::load("HUD.png")) {

    hProgress.setPosition(3.0f + hLabel.getWidth(), height - 52.0f);
    sProgress.setPosition(3.0f + sLabel.getWidth(), height - font->getSize());

    // create a valid data pointer
    crossHairVertices.reserve(24);

    glGenVertexArrays(1, &crossHairVAO);
    glGenBuffers(1, &crossHairVBO);

    glBindVertexArray(crossHairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crossHairVBO);
    glBufferData(GL_ARRAY_BUFFER,crossHairVertices.size() * sizeof(float), crossHairVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    glBindVertexArray(0);

    setHealth(100);
    setShield(100);
    resize(width, height);
}


void PlayerHud::render(const glm::mat4 &projection) {
    hLabel.render(projection);
    sLabel.render(projection);
    hProgress.render(projection);
    sProgress.render(projection);

    if (this->showEnemy) {
        eNameLabel.render(projection);
        eHealth.render(projection);
    }


    hudShader->use();
    texture->bind(GL_TEXTURE0);

    hudShader->setUniform("Texture", 0);
    hudShader->setUniform("projection", projection);
    hudShader->setUniform("visibility", 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(crossHairVAO);

    if (this->showEnemy) glDrawArrays(GL_TRIANGLES, 6, 6);
    else                 glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glDisable(GL_BLEND);
}

void PlayerHud::resize(float x, float y) {
    this->width = x;
    this->height = y;

    hLabel.setPosition(3.0f, y - 52.0f);
    sLabel.setPosition(3.0f, y - font->getSize());
    hProgress.setPosition(3.0f + hLabel.getWidth(), y - 52.0f);
    sProgress.setPosition(3.0f + sLabel.getWidth(), y - font->getSize()+2.0f);

    this->eNameLabel.setPosition(width/2-eNameLabel.getWidth()/2, font->getSize() + 10);
    this->eHealth.setPosition(width/2-eHealth.getWidth()/2, font->getSize()*2 + 15);

    this->crossHairVertices = {
            // position                                                     // texcoords:
            // Crosshair 1 (no enemy)
            x/2.0f - crossHairSize/2, y/2.0f - crossHairSize/2,            0.0f       , 0.0f       ,
            x/2.0f + crossHairSize/2, y/2.0f - crossHairSize/2,            0.11328125f, 0.0f       ,
            x/2.0f - crossHairSize/2, y/2.0f + crossHairSize/2,            0.0f       , 0.11328125f,
            x/2.0f + crossHairSize/2, y/2.0f - crossHairSize/2,            0.11328125f, 0.0f       ,
            x/2.0f + crossHairSize/2, y/2.0f + crossHairSize/2,            0.11328125f, 0.11328125f,
            x/2.0f - crossHairSize/2, y/2.0f + crossHairSize/2,            0.0f       , 0.11328125f,
            // Crosshair 2 (looking at enemy)
            x/2.0f - crossHairSize/2, y/2.0f - crossHairSize/2,            0.11328125f, 0.0f       ,
            x/2.0f + crossHairSize/2, y/2.0f - crossHairSize/2,            0.2265625f , 0.0f       ,
            x/2.0f - crossHairSize/2, y/2.0f + crossHairSize/2,            0.11328125f, 0.11328125f,
            x/2.0f + crossHairSize/2, y/2.0f - crossHairSize/2,            0.2265625f , 0.0f       ,
            x/2.0f + crossHairSize/2, y/2.0f + crossHairSize/2,            0.2265625f , 0.11328125f,
            x/2.0f - crossHairSize/2, y/2.0f + crossHairSize/2,            0.11328125f, 0.11328125f
    };

    glBindBuffer(GL_ARRAY_BUFFER, crossHairVBO);
    glBufferData(GL_ARRAY_BUFFER,crossHairVertices.size() * sizeof(float), crossHairVertices.data(), GL_STATIC_DRAW);
}

void PlayerHud::setHealth(int health) {
    if (this->health == health)
        return;

    char buffer[24];
    int len = snprintf(buffer, 24, "[========%3d%%========]", health);

    this->health = health;
    this->hProgress.setText(std::string(buffer, buffer+len));
}

void PlayerHud::setShield(int shield) {
    if (this->shield == shield)
        return;

    char buffer[18];
    int len = snprintf(buffer, 18, "[===%3d%%===]", shield);

    this->shield = shield;
    this->sProgress.setText(std::string(buffer, buffer+len));
}

void PlayerHud::setEnemy(std::string &name, int health, int maxHealth) {
    eNameLabel.setText(name);

    char buffer[32];
    int len = snprintf(buffer, 32, "[========%3d/%3d========]", health, maxHealth);

    this->eNameLabel.setText(name);
    this->eHealth.setText(std::string(buffer, buffer+len));

    this->eNameLabel.setPosition(width/2-eNameLabel.getWidth()/2, font->getSize() + 10);
    this->eHealth.setPosition(width/2-eHealth.getWidth()/2, font->getSize()*2 + 15);

    this->showEnemy = true;
}
