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

#define BUFFER_SIZE (32)

class PlayerHud : public Widget {

private:
    char buffer[BUFFER_SIZE];
    float crossHairSize = 58.0f;

    std::vector<float> hudVertices;
    std::vector<float> barVertices;
    std::vector<float> enemyVertices;

    GLuint hudVAO, hudVBO;
    GLuint barContentVAO, barContentVBO;
    GLuint enemyVAO, enemyVBO;

    std::shared_ptr<Font> font;
    std::shared_ptr<Shader> hudShader;
    std::shared_ptr<Texture> texture;

    Label eNameLabel;
    Label hProgress;
    Label sProgress;
    Label eHealth;

    std::string eNameBuffer;
    int eHealthBuffer = 0, maxHealthBuffer = 0;

    // TODO: Set in Constructor
    const int maxHealth = 100;
    const int maxShield = 100;

    int health = 0,shield = 0;
    float height, width;

    // TODO: Clean up the mess:
    // TODO: Maybe make static?
    const float iconS = 32.0f;
    const float iconX = 3.0f;
    const float hBsX = iconX + iconS + 3.0f;
    const float hBSize = 302.0f;
    const float hBHeight = 32.0f;
    const float sBSize = 140.0f;
    const float halfSize = crossHairSize/2.0f;

    void computeBarVBO();

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
