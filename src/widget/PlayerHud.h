//
// Created by Raphael on 15.04.2018.
//

#ifndef STAGE_FIGHTER_PLAYERHUD_H
#define STAGE_FIGHTER_PLAYERHUD_H

#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <string>
#include <memory>

#include "Widget.h"
#include "Label.h"
#include "../Texture.h"

class PlayerHud : public Widget {

private:
    float crossHairSize = 58.0f;
    std::vector<float> crossHairVertices;
    GLuint crossHairVAO, crossHairVBO;

    std::shared_ptr<Font> font;
    std::shared_ptr<Shader> hudShader;
    std::shared_ptr<Texture> texture;

    Label hLabel;
    Label sLabel;
    Label eNameLabel;

    //Progressbar or so?
    Label hProgress;
    Label sProgress;
    Label eHealth;

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
