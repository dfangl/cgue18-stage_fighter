//
// Created by raphael on 13.03.18.
//

#ifndef STAGE_FIGTHER_BULLETUNIVERSE_H
#define STAGE_FIGTHER_BULLETUNIVERSE_H

#include <memory>
#include <chrono>

#include <btBulletDynamicsCommon.h>

#include "bullet/GLDebugDrawer.h"

class BulletUniverse {

private:
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

    std::shared_ptr<GLDebugDrawer> debugDrawer;

public:
    explicit BulletUniverse(const btVector3 &gravity);
    ~BulletUniverse();

    void addRigidBody(btRigidBody *body);
    void removeRigidBody(btRigidBody *body);

    void addCollsionObject(btCollisionObject *body);
    void removeCollsipnObject(btCollisionObject *body);

    void simulate(std::chrono::duration<double, std::milli> tick);

    std::shared_ptr<GLDebugDrawer> getDebugDrawer() { return this->debugDrawer; }
    void enableDebugging();

    void drawDebug();

};


#endif //STAGE_FIGTHER_BULLETUNIVERSE_H
