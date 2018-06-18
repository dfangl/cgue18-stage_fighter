//
// Created by raphael on 16.04.18.
//

#ifndef STAGE_FIGHTER_PLAYER_H
#define STAGE_FIGHTER_PLAYER_H

#include <vector>
#include <memory>
#include <btBulletDynamicsCommon.h>

#include "../controller/CameraController.h"
#include "../widget/PlayerHud.h"
#include "../object3d/Model3DObject.h"
#include "../object3d/BulletObject.h"

class Player : public CameraEntity, public Logger {

private:
    int shield, maxShield;
    std::shared_ptr<PlayerHud> hud;

    std::shared_ptr<Entity> selectedEnemy = nullptr;
    Window *window;

    BulletObject weaponHurtBox;
    std::shared_ptr<Model3DObject> shieldModel;
    std::shared_ptr<Model3DObject> weaponModel;
    glm::mat4 animationMatrix = glm::mat4(1.0f);

    float oSpeed, oJumpSpeed;

    float shieldAnimationTime   = 0.0f;
    float shieldRegTime         = 0.0f;
    float shieldDamageTime      = 0.0f;

    float weaponAngle           = -25.0f;

    bool isBlocking = false;
    bool isHitting  = false;
    bool revertHitAnimation = false;

protected:
    struct ContactSensorCallback : public btCollisionWorld::ContactResultCallback {

        Player *player;
        btRigidBody *hurtBox;

        ContactSensorCallback(Player *player, btRigidBody *hurtBox)
                : btCollisionWorld::ContactResultCallback(), player(player), hurtBox(hurtBox) {}


        btScalar addSingleResult(btManifoldPoint& cp,
                                 const btCollisionObjectWrapper* colObj0, int partId0, int index0,
                                 const btCollisionObjectWrapper* colObj1, int partId1, int index1) override {

            BulletObject* other = nullptr;

            if (colObj0->m_collisionObject == hurtBox) {
                other =  static_cast<BulletObject *>(colObj1->m_collisionObject->getUserPointer());
            }
            if (colObj1->m_collisionObject == hurtBox) {
                other =  static_cast<BulletObject *>(colObj0->m_collisionObject->getUserPointer());
            }

            if (other == nullptr) {
                /* wtf? */
                player->logger->error("Wtf did happen to my hurtbox?");
                return 0;
            }

            if (player->isHitting)
            if (other->getKind() == BulletObject::ENEMY) {
                auto *enemy = dynamic_cast<Entity *>(other);
                player->revertHitAnimation = true;
                player->isHitting = false;
                player->hud->hitEnemyVisibility = 1.0f;
                enemy->receiveDamage(1);
            }

            return 0;
        }
    };

    ContactSensorCallback weaponHitCallback;

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
