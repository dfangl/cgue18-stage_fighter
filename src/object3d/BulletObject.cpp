//
// Created by Raphael on 17.03.2018.
//

#include "BulletObject.h"

BulletObject::BulletObject(const btVector3 &pos, const btQuaternion &rotation, btCollisionShape *bulletShape, btScalar mass) {
    btVector3 inertia(0, 0, 0);

    spdlog::get("console")->info("BulletObject: pos:{},{},{}, rot:{},{},{},{}", pos.x(), pos.y(), pos.z(), rotation.x(), rotation.y(), rotation.z(), rotation.w());

    this->fallShape = bulletShape;
    this->mass = mass;

    if (this->mass > 0)
        this->fallShape->calculateLocalInertia(mass, inertia);

    this->motionState = new btDefaultMotionState(btTransform(rotation, pos));
    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, this->motionState, fallShape, inertia);
    this->rigidBody = new btRigidBody(rigidBodyCI);

    if (this->mass <= 0) {
        this->rigidBody->setFlags(
                this->rigidBody->getFlags() |  btCollisionObject::CF_KINEMATIC_OBJECT
        );
    }
}

BulletObject::~BulletObject() {
    delete this->rigidBody->getMotionState();
    delete this->rigidBody;

    delete this->fallShape;
}

void BulletObject::setOrigin(const btVector3 &position, const btQuaternion &rotation) {
    btTransform initialTransform;

    initialTransform.setOrigin(position);
    initialTransform.setRotation(rotation);

    this->rigidBody->setWorldTransform(initialTransform);
    this->motionState->setWorldTransform(initialTransform);
}

btTransform BulletObject::getTransformation() {
    btTransform trans;
    this->rigidBody->getMotionState()->getWorldTransform(trans);

    return trans;
}