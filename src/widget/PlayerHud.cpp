//
// Created by Raphael on 15.04.2018.
//

#include "PlayerHud.h"
#include "../manager/ShaderManager.h"
#include "../manager/TextureManager.h"

PlayerHud::PlayerHud(std::shared_ptr<Font> font, float width, float height) :
    font(font), hudShader(ShaderManager::load("hud")),
    hProgress(std::string(""), font, 1.0f, 1.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f)),
    sProgress(std::string(""), font, 1.0f, 1.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f)),
    eNameLabel(std::string(""), font, 1.0f, 1.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f)),
    eHealth(std::string(""), font, 1.0f, 1.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f)),
    height(height), width(width), texture(TextureManager::load("HUD.png")) {

    // create a valid data pointer
    hudVertices.reserve(36);
    barVertices.reserve(12);
    enemyVertices.reserve(12);

    // HUD:
    glGenVertexArrays(1, &hudVAO);
    glGenBuffers(1, &hudVBO);

    glBindVertexArray(hudVAO);
    glBindBuffer(GL_ARRAY_BUFFER, hudVBO);
    glBufferData(GL_ARRAY_BUFFER,hudVertices.size() * sizeof(float), hudVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    glBindVertexArray(0);

    // Bar:
    glGenVertexArrays(1, &barContentVAO);
    glGenBuffers(1, &barContentVBO);

    glBindVertexArray(barContentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, barContentVBO);
    glBufferData(GL_ARRAY_BUFFER,barVertices.size() * sizeof(float), barVertices.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    glBindVertexArray(0);

    //Enemy:
    glGenVertexArrays(1, &enemyVAO);
    glGenBuffers(1, &enemyVBO);

    glBindVertexArray(enemyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, enemyVBO);
    glBufferData(GL_ARRAY_BUFFER, enemyVertices.size() * sizeof(float), enemyVertices.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    glBindVertexArray(0);


    setHealth(100);
    setShield(100);

    this->resize(width, height);
}


void PlayerHud::render(const glm::mat4 &projection) {
    hudShader->use();
    texture->bind(GL_TEXTURE0);

    hudShader->setUniform("Texture", 0);
    hudShader->setUniform("projection", projection);
    hudShader->setUniform("visibility", 1.0f);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (this->showEnemy) {
        glBindVertexArray(enemyVAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);
        glBindVertexArray(0);
    }

    // Draw Content of Bars:
    glBindVertexArray(barContentVAO);
    glDrawArrays(GL_TRIANGLES, 0, 12);
    glBindVertexArray(0);

    glBindVertexArray(hudVAO);

    if (this->showEnemy) glDrawArrays(GL_TRIANGLES, 6, 6);
    else                 glDrawArrays(GL_TRIANGLES, 0, 6);

    // Draw Health & Shield bar:
    glDrawArrays(GL_TRIANGLES, 12, 24);
    glBindVertexArray(0);

    if (this->showEnemy) {
        eNameLabel.render(projection);
        eHealth.render(projection);
    }

    hProgress.render(projection);
    sProgress.render(projection); // already does glDisable(GL_BLEND)
}

void PlayerHud::resize(float x, float y) {
    //TODO: Clean up the mess:
    const float halfX = x/2.0f;
    const float halfY = y/2.0f;
    const float hBsY = y - 52.0f - font->getSize();
    const float sBsY = y - 18.0f - font->getSize();
    const float hIconY = y - 52.0f - font->getSize();
    const float sIconY = y - 18.0f - font->getSize();

    this->width = x;
    this->height = y;

    hProgress.setPosition(hBsX + hBSize/2.0f - hProgress.getWidth()/2.0f, y - 52.0f);
    sProgress.setPosition(hBsX + sBSize/2.0f - sProgress.getWidth()/2.0f, y - font->getSize() + 18.0f/2.0f - 2.0f);

    this->eNameLabel.setPosition(width/2-eNameLabel.getWidth()/2, font->getSize() + 10);
    this->eHealth.setPosition(width/2-eHealth.getWidth()/2, font->getSize()*2 + 15);

    this->hudVertices = {
            // position                          // texcoords: (Crosshair is 58x58 px on texture)
            // Crosshair 1 (no enemy)
            halfX - halfSize, halfY - halfSize, texture->pixelWidthToNormal(  0), texture->pixelHeightToNormal(  0),
            halfX + halfSize, halfY - halfSize, texture->pixelWidthToNormal( 58), texture->pixelHeightToNormal(  0),
            halfX - halfSize, halfY + halfSize, texture->pixelWidthToNormal(  0), texture->pixelHeightToNormal( 58),
            halfX + halfSize, halfY - halfSize, texture->pixelWidthToNormal( 58), texture->pixelHeightToNormal(  0),
            halfX + halfSize, halfY + halfSize, texture->pixelWidthToNormal( 58), texture->pixelHeightToNormal( 58),
            halfX - halfSize, halfY + halfSize, texture->pixelWidthToNormal(  0), texture->pixelHeightToNormal( 58),
            // Crosshair 2 (looking at enemy)
            halfX - halfSize, halfY - halfSize, texture->pixelWidthToNormal( 58), texture->pixelHeightToNormal(  0),
            halfX + halfSize, halfY - halfSize, texture->pixelWidthToNormal(116), texture->pixelHeightToNormal(  0),
            halfX - halfSize, halfY + halfSize, texture->pixelWidthToNormal( 58), texture->pixelHeightToNormal( 58),
            halfX + halfSize, halfY - halfSize, texture->pixelWidthToNormal(116), texture->pixelHeightToNormal(  0),
            halfX + halfSize, halfY + halfSize, texture->pixelWidthToNormal(116), texture->pixelHeightToNormal( 58),
            halfX - halfSize, halfY + halfSize, texture->pixelWidthToNormal( 58), texture->pixelHeightToNormal( 58),
            // Health:
            hBsX           , hBsY           , texture->pixelWidthToNormal(  0), texture->pixelHeightToNormal( 60),
            hBsX + hBSize  , hBsY           , texture->pixelWidthToNormal(512), texture->pixelHeightToNormal( 60),
            hBsX           , hBsY + hBHeight, texture->pixelWidthToNormal(  0), texture->pixelHeightToNormal(102),
            hBsX + hBSize  , hBsY           , texture->pixelWidthToNormal(512), texture->pixelHeightToNormal( 60),
            hBsX + hBSize  , hBsY + hBHeight, texture->pixelWidthToNormal(512), texture->pixelHeightToNormal(102),
            hBsX           , hBsY + hBHeight, texture->pixelWidthToNormal(  0), texture->pixelHeightToNormal(102),
            // Shield:
            hBsX           , sBsY           , texture->pixelWidthToNormal(  0), texture->pixelHeightToNormal( 60),
            hBsX + sBSize  , sBsY           , texture->pixelWidthToNormal(512), texture->pixelHeightToNormal( 60),
            hBsX           , sBsY + hBHeight, texture->pixelWidthToNormal(  0), texture->pixelHeightToNormal(102),
            hBsX + sBSize  , sBsY           , texture->pixelWidthToNormal(512), texture->pixelHeightToNormal( 60),
            hBsX + sBSize  , sBsY + hBHeight, texture->pixelWidthToNormal(512), texture->pixelHeightToNormal(102),
            hBsX           , sBsY + hBHeight, texture->pixelWidthToNormal(  0), texture->pixelHeightToNormal(102),
            // Heart-Icon: (64x64)
            iconX           , hIconY           , texture->pixelWidthToNormal(  0), texture->pixelHeightToNormal(448),
            iconX + iconS   , hIconY           , texture->pixelWidthToNormal( 64), texture->pixelHeightToNormal(448),
            iconX           , hIconY + iconS   , texture->pixelWidthToNormal(  0), texture->pixelHeightToNormal(512),
            iconX + iconS   , hIconY           , texture->pixelWidthToNormal( 64), texture->pixelHeightToNormal(448),
            iconX + iconS   , hIconY + iconS   , texture->pixelWidthToNormal( 64), texture->pixelHeightToNormal(512),
            iconX           , hIconY + iconS   , texture->pixelWidthToNormal(  0), texture->pixelHeightToNormal(512),
            // Shield-Icon: (64x64)
            iconX           , sIconY           , texture->pixelWidthToNormal( 64), texture->pixelHeightToNormal(448),
            iconX + iconS   , sIconY           , texture->pixelWidthToNormal(128), texture->pixelHeightToNormal(448),
            iconX           , sIconY + iconS   , texture->pixelWidthToNormal( 64), texture->pixelHeightToNormal(512),
            iconX + iconS   , sIconY           , texture->pixelWidthToNormal(128), texture->pixelHeightToNormal(448),
            iconX + iconS   , sIconY + iconS   , texture->pixelWidthToNormal(128), texture->pixelHeightToNormal(512),
            iconX           , sIconY + iconS   , texture->pixelWidthToNormal( 64), texture->pixelHeightToNormal(512)
    };

    glBindBuffer(GL_ARRAY_BUFFER, hudVBO);
    glBufferData(GL_ARRAY_BUFFER,hudVertices.size() * sizeof(float), hudVertices.data(), GL_STATIC_DRAW);

    computeBarVBO();
}

void PlayerHud::setHealth(int health) {
    if (this->health == health || health < 0)
        return;

    int len = snprintf(buffer, BUFFER_SIZE, "%3d%%", health);

    this->health = health;
    this->hProgress.setText(std::string(buffer, buffer+len));
    this->hProgress.setPosition(hBsX + hBSize/2.0f - hProgress.getWidth()/2.0f, height - 52.0f);
    computeBarVBO();
}

void PlayerHud::setShield(int shield) {
    if (this->shield == shield || shield < 0)
        return;

    int len = snprintf(buffer, BUFFER_SIZE, "%3d%%", shield);

    this->shield = shield;
    this->sProgress.setText(std::string(buffer, buffer+len));
    this->sProgress.setPosition(hBsX + sBSize/2.0f - sProgress.getWidth()/2.0f, height - font->getSize() + 18.0f/2.0f - 2.0f);
    computeBarVBO();
}

void PlayerHud::setEnemy(std::string &name, int health, int maxHealth) {
    if(this->eNameBuffer.compare(name) != 0) {
        this->eNameBuffer = name;

        eNameLabel.setText(name);
        eNameLabel.setPosition(width/2-eNameLabel.getWidth()/2, font->getSize() + 10);
    }

    if (this->eHealthBuffer != health || this->maxHealthBuffer != maxHealth) {
        this->eHealthBuffer = health;
        this->maxHealthBuffer = maxHealth;

        const int len = snprintf(buffer, BUFFER_SIZE, "%3d / %3d", health, maxHealth);

        eHealth.setText(std::string(buffer, buffer+len));
        eHealth.setPosition(width/2-eHealth.getWidth()/2, font->getSize()*2 + 15);
    }

    this->showEnemy = true;

    const float hBsY = font->getSize()*2 - 8;
    const float hBsX = width/2.0f - hBSize/2.0f;
    const float hPer = (float) health / (float) maxHealth;

    this->enemyVertices = {
            // Health:
            hBsX                 , hBsY           , texture->pixelWidthToNormal(  0)     , texture->pixelHeightToNormal(104),
            hBsX + hBSize * hPer , hBsY           , texture->pixelWidthToNormal(512*hPer), texture->pixelHeightToNormal(142),
            hBsX                 , hBsY + hBHeight, texture->pixelWidthToNormal(  0)     , texture->pixelHeightToNormal(104),
            hBsX + hBSize * hPer , hBsY           , texture->pixelWidthToNormal(512*hPer), texture->pixelHeightToNormal(104),
            hBsX + hBSize * hPer , hBsY + hBHeight, texture->pixelWidthToNormal(512*hPer), texture->pixelHeightToNormal(142),
            hBsX                 , hBsY + hBHeight, texture->pixelWidthToNormal(  0)     , texture->pixelHeightToNormal(142),
            hBsX           , hBsY           , texture->pixelWidthToNormal(  0), texture->pixelHeightToNormal( 60),
            hBsX + hBSize  , hBsY           , texture->pixelWidthToNormal(512), texture->pixelHeightToNormal( 60),
            hBsX           , hBsY + hBHeight, texture->pixelWidthToNormal(  0), texture->pixelHeightToNormal(102),
            hBsX + hBSize  , hBsY           , texture->pixelWidthToNormal(512), texture->pixelHeightToNormal( 60),
            hBsX + hBSize  , hBsY + hBHeight, texture->pixelWidthToNormal(512), texture->pixelHeightToNormal(102),
            hBsX           , hBsY + hBHeight, texture->pixelWidthToNormal(  0), texture->pixelHeightToNormal(102),
    };

    glBindBuffer(GL_ARRAY_BUFFER, enemyVBO);
    glBufferData(GL_ARRAY_BUFFER, enemyVertices.size() * sizeof(float), enemyVertices.data(), GL_DYNAMIC_DRAW);
}

void PlayerHud::computeBarVBO() {
    const float hBsY = height - 52.0f - font->getSize();
    const float sBsY = height - 18.0f - font->getSize();

    const float hPer = (float) health / (float)maxHealth;
    const float sPer = (float)shield / (float)maxShield;

    this->barVertices = {
            // Health:
            hBsX                 , hBsY           , texture->pixelWidthToNormal(  0)     , texture->pixelHeightToNormal(104),
            hBsX + hBSize * hPer , hBsY           , texture->pixelWidthToNormal(512*hPer), texture->pixelHeightToNormal(142),
            hBsX                 , hBsY + hBHeight, texture->pixelWidthToNormal(  0)     , texture->pixelHeightToNormal(104),
            hBsX + hBSize * hPer , hBsY           , texture->pixelWidthToNormal(512*hPer), texture->pixelHeightToNormal(104),
            hBsX + hBSize * hPer , hBsY + hBHeight, texture->pixelWidthToNormal(512*hPer), texture->pixelHeightToNormal(142),
            hBsX                 , hBsY + hBHeight, texture->pixelWidthToNormal(  0)     , texture->pixelHeightToNormal(142),
            // Shield:
            hBsX                 , sBsY           , texture->pixelWidthToNormal(  0)     , texture->pixelHeightToNormal(146),
            hBsX + sBSize * sPer , sBsY           , texture->pixelWidthToNormal(512*sPer), texture->pixelHeightToNormal(184),
            hBsX                 , sBsY + hBHeight, texture->pixelWidthToNormal(  0)     , texture->pixelHeightToNormal(146),
            hBsX + sBSize * sPer , sBsY           , texture->pixelWidthToNormal(512*sPer), texture->pixelHeightToNormal(146),
            hBsX + sBSize * sPer , sBsY + hBHeight, texture->pixelWidthToNormal(512*sPer), texture->pixelHeightToNormal(184),
            hBsX                 , sBsY + hBHeight, texture->pixelWidthToNormal(  0)     , texture->pixelHeightToNormal(184),
    };

    glBindBuffer(GL_ARRAY_BUFFER, barContentVBO);
    glBufferData(GL_ARRAY_BUFFER, barVertices.size() * sizeof(float), barVertices.data(), GL_DYNAMIC_DRAW);
}
