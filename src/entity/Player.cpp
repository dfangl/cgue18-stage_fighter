//
// Created by raphael on 16.04.18.
//

#include <cfloat>

#include "Player.h"

#include "../manager/FontManager.h"
#include "../manager/ModelManager.h"

#include "BulletEntity.h"
#include "ScriptedEntity.h"

#include "../helper/QuatUtils.h"

Player::Player(Camera &camera, Window *window, const std::shared_ptr<BulletUniverse> &world) :
        Logger("Player"),
        CameraEntity(camera, world, new btSphereShape(0.7f), 1.0f),
        hud(std::make_shared<PlayerHud>(FontManager::get("Lato-24"), camera.getViewPort().z, camera.getViewPort().w))
        {

    this->window = window;

    this->health = 100;
    this->maxHealth = 100;

    this->shield = 0;
    this->maxShield = 100;

    this->entitySpeed = 5.5f;

    this->shieldModel = std::make_shared<Model3DObject>(
            camera.getPosition(),
            1.0f,
            ModelManager::load("shield"),
            ShaderManager::load("standard")
    );

    this->shieldModel->enableAnimation(Model3DObject::Animation(
        std::string("Cube_CubeAction"), 0.0f, 1.25f, true
    ));


}

void Player::think(std::chrono::duration<double, std::milli> delta) {
    CameraEntity::think(delta);

    shieldModel->setOrigin(camera.getPosition());
    //shieldModel->setRotation(Quat::toQuaternion(camera.getPitch(), 90.0f, camera.getYaw()));
    //shieldAnimationTime += delta.count() / 1000;
    //this->shieldModel->applyAnimation(shieldAnimationTime);
    opengl_check_error(spdlog::get("console"), "Player: think()");

    hud->setHealth(health);
    hud->setShield(shield);
}

void Player::computeEnemyInView(std::vector<std::shared_ptr<Entity>> &entities) {
    const auto width = this->camera.getViewPort().z;
    const auto height = this->camera.getViewPort().w;

    selectedEnemy = nullptr;
    float targetDist = FLT_MAX;

    for (auto &entity : entities) {
        if (entity->getEntityKind() != BulletObject::ENEMY)
            continue;

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
        this->health -= 1;
    }
}

BulletObject::Kind Player::getEntityKind() {
    return BulletObject::PLAYER;
}

void Player::render(Scene *scene) {
    this->shieldModel->render(scene);
}
