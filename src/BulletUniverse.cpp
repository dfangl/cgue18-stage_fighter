//
// Created by raphael on 13.03.18.
//

#include "BulletUniverse.h"

BulletUniverse::BulletUniverse(const btVector3 &gravity) {
    this->broadphase = new btDbvtBroadphase();
    this->collisionConfiguration = new btDefaultCollisionConfiguration();
    this->dispatcher = new btCollisionDispatcher(collisionConfiguration);
    this->solver = new btSequentialImpulseConstraintSolver;

    this->dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    this->dynamicsWorld->setGravity(gravity);
}

BulletUniverse::~BulletUniverse() {
    delete dynamicsWorld;
    delete solver;
    delete collisionConfiguration;
    delete dispatcher;
    delete broadphase;
}

void BulletUniverse::addRigidBody(btRigidBody *body) {
    this->dynamicsWorld->addRigidBody(body);
}

void BulletUniverse::simulate(std::chrono::milliseconds tick) {
    dynamicsWorld->stepSimulation(tick.count() / 1000.0f, 10);
}

void BulletUniverse::removeRigidBody(btRigidBody *body) {
    this->dynamicsWorld->removeRigidBody(body);
}
