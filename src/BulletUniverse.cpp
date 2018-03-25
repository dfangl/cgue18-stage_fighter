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

void BulletUniverse::simulate(std::chrono::duration<double, std::milli> tick) {
    dynamicsWorld->stepSimulation(static_cast<btScalar>(tick.count() / 1000.f), 10);
}

void BulletUniverse::removeRigidBody(btRigidBody *body) {
    this->dynamicsWorld->removeRigidBody(body);
}

void BulletUniverse::addCollsipnObject(btCollisionObject *body) {
    this->dynamicsWorld->addCollisionObject(body);
}

void BulletUniverse::removeCollsipnObject(btCollisionObject *body) {
    this->dynamicsWorld->removeCollisionObject(body);
}
