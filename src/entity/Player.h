//
// Created by raphael on 16.04.18.
//

#ifndef STAGE_FIGHTER_PLAYER_H
#define STAGE_FIGHTER_PLAYER_H

#include <vector>
#include <memory>

#include "../controller/CameraController.h"
#include "../widget/PlayerHud.h"

class Player : public CameraEntity, public Logger {

private:
    int shield, maxShield;
    std::shared_ptr<PlayerHud> hud;

    std::shared_ptr<Entity> selectedEnemy = nullptr;

public:
    Player(Camera &camera, const std::shared_ptr<BulletUniverse> &world);

    std::shared_ptr<PlayerHud> &getHud() { return this->hud; }

    void think(std::chrono::duration<double, std::milli> delta) override;
    void computeEnemyInView(std::vector<std::shared_ptr<Entity>> &entities);

    void collideWith(BulletObject *other) override;
};


#endif //STAGE_FIGHTER_PLAYER_H
