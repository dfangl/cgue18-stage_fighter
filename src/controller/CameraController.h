//
// Created by Raphael on 17.03.2018.
//

#ifndef STAGE_FIGHTER_CAMERAENTITY_H
#define STAGE_FIGHTER_CAMERAENTITY_H

#include <functional>
#include "../Window.h"

#include "../object3d/BulletObject.h"

#include "../entity/Entity.h"
#include "../BulletUniverse.h"

class CameraEntity : public Entity, public BulletObject {

private:
    btVector3 speed;
    float height = 0.7f; // Capsule height * 2 [radius]
    const float airTime = 50.0f; // in ms

    bool forewardPressed = false;
    bool backwardPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;

    float jump = 0.0f;
    bool enabled = true;

    std::shared_ptr<BulletUniverse> world;
    std::function<void(Window const *)> keyboardCallback;

    static btVector3 bulletMovementVector;

protected:
    Camera &camera;

    bool canJump = false;
    glm::mat4 modelMatrix;
    btVector3 stickyVelocity;

public:
    explicit CameraEntity(Camera &camera,
                          const std::shared_ptr<BulletUniverse> &world,
                          btCollisionShape *bulletShape = new btBoxShape(btVector3(0.7f, 0.7, 0.7f)),
                          btScalar mass = 1.0f);

    ~CameraEntity() override;

    float entitySpeed = 7.0f;
    float jumpSpeed = 14.0f;

    void enable();
    void disable();

    void setPosition(const glm::vec3 &vec, const glm::quat &rot) override;
    void lookAt(const glm::vec3 &obj);

    void think(std::chrono::duration<double, std::milli> delta) override;

    std::function<void(Window const *)> getKeyboardCallback() const { return this->keyboardCallback; }

    void collideWith(BulletObject *other) override;

    virtual void render(Scene *scene) = 0;

    glm::vec3 isInView(const Entity *entity);

    float getBoundingSphereRadius() override;

    const glm::vec3 &getPosition() const override;

};


#endif //STAGE_FIGHTER_CAMERAENTITY_H
