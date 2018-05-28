//
// Created by raphael on 16.04.18.
//

#include <cfloat>

#include "Player.h"
#include "../manager/FontManager.h"
#include "BulletEntity.h"
#include "ScriptedEntity.h"

Player::Player(Camera &camera, Window *window, const std::shared_ptr<BulletUniverse> &world) :
        CameraEntity(camera, world, new btSphereShape(0.7f), 1.0f),
        hud(std::make_shared<PlayerHud>(FontManager::get("Lato-24"), camera.getViewPort().z, camera.getViewPort().w)),
        Logger("Player") {

    this->window = window;

    this->health = 100;
    this->maxHealth = 100;

    this->shield = 0;
    this->maxShield = 100;

    this->entitySpeed = 5.5f;

    //BulletObject::rigidBody->setCcdMotionThreshold(7.0f);
    //BulletObject::rigidBody->setCcdSweptSphereRadius(0.5f);
}

void Player::think(std::chrono::duration<double, std::milli> delta) {
    CameraEntity::think(delta);

    //for (auto &kD : this->knockbackDirections)
    //    BulletObject::rigidBody->applyCentralImpulse(btVector3(kD.x * 4, kD.y * 2, kD.z * 4));

    hud->setHealth(health);
    hud->setShield(shield);
    //this->knockbackDirections.clear();
}

void Player::computeEnemyInView(std::vector<std::shared_ptr<Entity>> &entities) {
    const auto width = this->camera.getViewPort().z;
    const auto height = this->camera.getViewPort().w;

    selectedEnemy = nullptr;
    float targetDist = FLT_MAX;

    for (auto &entity : entities) {
        // TODO: Check if kind == Enemy

        const glm::vec3 distV = glm::abs(getPosition() - entity->getPosition());
        const auto distance =  distV.x + distV.y + distV.z;

        if (distance < 25.0f) {
            auto p = isInView(entity.get());
            if (p.x > width/2-width/4 && p.x < width/2+width/4 &&
                p.y > height/2-height/4 && p.y < height/2+height/4 &&
                targetDist > distance && p.z < 1.0) {
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

    if (other->getKind() == BulletObject::ENEMY) {
        if (window->getMouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
            logger->info("Hit with Enemy ({})!", (void*)other);

            auto *enemy = dynamic_cast<Entity *>(other);
            enemy->receiveDamage(1);
        }
    }

    if (other->getKind() == BulletObject::BULLET) {
        //const auto *bullet = (const BulletEntity *)(other);
        //const glm::vec3 direction = glm::normalize(position - bullet->getEntityPosition());
        //this->knockbackDirections.push_back(direction);

        this->health -= 1;

    }
}
