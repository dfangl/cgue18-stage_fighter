//
// Created by Raphael on 17.03.2018.
//

#include "CameraController.h"
#include "../manager/TextureManager.h"
#include <GLFW/glfw3.h>


btVector3 CameraEntity::bulletMovementVector(0, 1, 0);

CameraEntity::CameraEntity(Camera &camera, std::shared_ptr<BulletUniverse> world, btCollisionShape *bulletShape, btScalar mass) :
        BulletObject(btVector3(camera.getPosition().x,camera.getPosition().y,camera.getPosition().z), btQuaternion(0,0,0,1), bulletShape, mass), camera(camera){
    this->speed = btVector3(0.0, 0.0, 0.0);
    this->keyboardCallback = [this](Window const *window) {
        this->forewardPressed  = window->getKey(GLFW_KEY_W) != GLFW_RELEASE;
        this->leftPressed      = window->getKey(GLFW_KEY_A) != GLFW_RELEASE;
        this->backwardPressed  = window->getKey(GLFW_KEY_S) != GLFW_RELEASE;
        this->rightPressed     = window->getKey(GLFW_KEY_D) != GLFW_RELEASE;
        this->jump             = window->getKey(GLFW_KEY_SPACE) == GLFW_PRESS;
    };

    rigidBody->setActivationState(DISABLE_DEACTIVATION);
    rigidBody->setFriction(1.0f);
    rigidBody->setRestitution(0.1f);

    world->addRigidBody(rigidBody);
}

void CameraEntity::setEntityPosition(const glm::vec3 &vec) {
    this->camera.update(vec);
    BulletObject::setOrigin(btVector3(vec.x,vec.y,vec.z), btQuaternion(0,0,0,1));
}

void CameraEntity::think(std::chrono::duration<double, std::milli> delta) {
    if (!enabled)
        return;

    auto bT = this->getTransformation();
    auto o = bT.getOrigin();
    this->camera.update(glm::vec3(o.x(), o.y()+0.5, o.z()));

    float zVelocity = 0.0f;
    float xVelocity = 0.0f;

    if (this->forewardPressed) xVelocity =  this->entitySpeed;
    if (this->backwardPressed) xVelocity = -this->entitySpeed;
    if (this->leftPressed)     zVelocity = -this->entitySpeed;
    if (this->rightPressed)    zVelocity =  this->entitySpeed;

    speed.setZ(zVelocity);
    speed.setX(xVelocity);
    speed.setY(0.0);

    if (jump) {
        speed.setY(this->entitySpeed * 9.81f);
        jump = false;
    }

    if(!forewardPressed && !backwardPressed && !leftPressed && !rightPressed) {
        rigidBody->setDamping(200.0f, 0.0f);
    } else {
        rigidBody->setDamping(0.0f, 0.0f);
    }


    //if (o.y() > 0.99999f) speed.setY(-9.81f);
    //else                speed.setY(0.0f);

    //rigidBody->setLinearVelocity(speed.rotate(bulletMovementVector, btRadians(-camera.getYaw())));
    rigidBody->applyCentralForce(speed.rotate(bulletMovementVector, btRadians(-camera.getYaw())));
}

CameraEntity::~CameraEntity() {
    world->removeRigidBody(BulletObject::rigidBody);
}
