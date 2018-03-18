//
// Created by Raphael on 17.03.2018.
//

#include "CameraEntity.h"
#include "../manager/TextureManager.h"
#include <GLFW/glfw3.h>

btVector3 CameraEntity::bulletMovementVector(0, 1, 0);

#include <iostream>
CameraEntity::CameraEntity(Camera &camera, std::shared_ptr<BulletUniverse> world, btCollisionShape *bulletShape, btScalar mass) :
        BulletObject(btVector3(camera.getPosition().x,camera.getPosition().y,camera.getPosition().z), btQuaternion(0,0,0,1), bulletShape, mass), camera(camera){
    this->speed = btVector3(0.0, 0.0, 0.0);
    this->keyboardCallback = [this](int key, int scancode, int action ,int mode) {
        switch(key) {
            case GLFW_KEY_W: this->forewardPressed  = action != GLFW_RELEASE; break;
            case GLFW_KEY_A: this->leftPressed      = action != GLFW_RELEASE; break;
            case GLFW_KEY_S: this->backwardPressed  = action != GLFW_RELEASE; break;
            case GLFW_KEY_D: this->rightPressed     = action != GLFW_RELEASE; break;
            case GLFW_KEY_SPACE: this->jump         = action == GLFW_PRESS; break;
            default:; /* nothing to do ... */
        }
    };

    rigidBody->setActivationState(DISABLE_DEACTIVATION);
    rigidBody->setFriction(1.0f);
    rigidBody->setRestitution(100.0f);

    world->addRigidBody(rigidBody);
    //rigidBody->setFlags(rigidBody->getCollisionFlags()|btCollisionObject::CF_KINEMATIC_OBJECT);
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
    speed.setY(0.0f);

    if (o.y() > 0.99999f) speed.setY(-9.81f);
    //else                speed.setY(0.0f);

    //this->rigidBody->activate(true);
    rigidBody->setAngularFactor(bulletMovementVector);
    rigidBody->setLinearVelocity(speed.rotate(bulletMovementVector, btRadians(-camera.getYaw())));

    if(jump) {
        rigidBody->applyCentralImpulse(btVector3(0, 9.81f * this->entitySpeed, 0));
        jump = false;
    }
}

CameraEntity::~CameraEntity() {
    world->removeRigidBody(BulletObject::rigidBody);
}
