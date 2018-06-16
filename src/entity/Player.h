//
// Created by raphael on 16.04.18.
//

#ifndef STAGE_FIGHTER_PLAYER_H
#define STAGE_FIGHTER_PLAYER_H

#include <vector>
#include <memory>

#include "../controller/CameraController.h"
#include "../widget/PlayerHud.h"
#include "../object3d/Model3DObject.h"

class Player : public CameraEntity, public Logger {

private:
    int shield, maxShield;
    std::shared_ptr<PlayerHud> hud;

    std::shared_ptr<Entity> selectedEnemy = nullptr;
    Window *window;

    std::shared_ptr<Model3DObject> shieldModel;
    std::shared_ptr<Model3DObject> weaponModel;
    glm::mat4 animationMatrix = glm::mat4(1.0f);

    float shieldAnimationTime = 0.0f;
    float shieldRegTime = 0.0f;
    float shieldDamageTime = 0.0f;
    bool isBlocking = false;
    float oSpeed, oJumpSpeed;
    float weaponAngle = -25.0f;

public:
    Player(Camera &camera, Window *window, const std::shared_ptr<BulletUniverse> &world);
    ~Player() override = default;

    std::shared_ptr<PlayerHud> &getHud() { return this->hud; }

    void think(std::chrono::duration<double, std::milli> delta) override;
    void computeEnemyInView(std::vector<std::shared_ptr<Entity>> &entities);

    void collideWith(BulletObject *other) override;

    void render(Scene *scene) override;

    Kind getEntityKind() override;

    void setCrosshairState(bool show);
};


#endif //STAGE_FIGHTER_PLAYER_H
