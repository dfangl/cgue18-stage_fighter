//
// Created by raphael on 16.04.18.
//

#include <cfloat>

#include "Player.h"
#include "../manager/FontManager.h"

Player::Player(Camera &camera, const std::shared_ptr<BulletUniverse> &world) :
        CameraEntity(camera, world, new btSphereShape(0.7f), 1.0f),
        hud(std::make_shared<PlayerHud>(FontManager::load("Lato-24"), camera.getViewPort().z, camera.getViewPort().w)),
        Logger("Player") {

    this->health = 100;
    this->maxHealth = 100;

    this->shield = 0;
    this->maxShield = 100;

    this->entitySpeed = 5.5f;

    BulletObject::rigidBody->setCcdMotionThreshold(7.0f);
    BulletObject::rigidBody->setCcdSweptSphereRadius(0.5f);
}

void Player::think(std::chrono::duration<double, std::milli> delta) {
    CameraEntity::think(delta);

    hud->setHealth(health);
    hud->setShield(shield);
}

void Player::computeEnemyInView(std::vector<std::shared_ptr<Entity>> &entities) {
    const auto width = this->camera.getViewPort().z;
    const auto height = this->camera.getViewPort().w;

    selectedEnemy = nullptr;
    float targetDist = FLT_MAX;

    for (auto &entity : entities) {
        // TODO: Check if kind == Enemy

        const glm::vec3 distV = glm::abs(getEntityPosition() - entity->getEntityPosition());
        const auto distance =  distV.x + distV.y + distV.z;

        if (distance < 25.0f) {
            auto p = isInView(entity.get());
            if (p.x > width/2-width/5 && p.x < width/2+width/5 &&
                p.y > height/2-height/5 && p.y < height/2+height/5 &&
                targetDist > distance) {
                selectedEnemy = entity;
            }
        }
    }

    if (selectedEnemy != nullptr) {
        auto name = selectedEnemy->getName();
        hud->setEnemy(name, selectedEnemy->getHealth(), selectedEnemy->getMaxHealth());
    } else {
        hud->showEnemy = false;
        selectedEnemy.reset();
    }
}

void Player::collideWith(BulletObject *other) {
    if (other->getKind() == BulletObject::ENVIRONMENT)
        return;

    if (other->getKind() == BulletObject::BULLET)
        this->health -= 5;
}
