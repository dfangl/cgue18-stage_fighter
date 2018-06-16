//
// Created by raphael on 13.03.18.
//

#ifndef STAGE_FIGTHER_BULLETUNIVERSE_H
#define STAGE_FIGTHER_BULLETUNIVERSE_H

#include <memory>
#include <chrono>

#include <btBulletDynamicsCommon.h>

#include "bullet/GLDebugDrawer.h"

/**
 * A Simple Wrapper Class for the Bullet World which does the heavy lifting for the initalization and the
 * Debug Context handling ...
 */
class BulletUniverse {

private:
    btBroadphaseInterface* broadphase                       = nullptr;
    btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
    btCollisionDispatcher* dispatcher                       = nullptr;
    btSequentialImpulseConstraintSolver* solver             = nullptr;
    btDiscreteDynamicsWorld* dynamicsWorld                  = nullptr;

    bool debug                                              = false;
    std::shared_ptr<GLDebugDrawer> debugDrawer              = nullptr;
    double internalTick                                     = 0.0;

public:
    explicit BulletUniverse(const btVector3 &gravity);
    BulletUniverse(BulletUniverse&) = delete;
    BulletUniverse &operator=(BulletUniverse&) = delete;
    ~BulletUniverse();

    void addRigidBody(btRigidBody *body);
    void removeRigidBody(btRigidBody *body);

    /**
     * Simulate the Bullet World, tick is the time in ms which passed since the last
     * call of this function
     *
     * @param tick Delta T
     */
    void simulate(std::chrono::duration<double, std::milli> tick);

    std::shared_ptr<GLDebugDrawer> &getDebugDrawer() { return this->debugDrawer; }
    void enableDebugging();
    bool isDebugging() const { return debug; }

    void drawDebug();
    void rayTest(const btVector3 &start, const btVector3 &end, btCollisionWorld::ClosestRayResultCallback &rayCallback);

    /*
     * Flag is only applied to new instances, use enableDebugging on existing instances to start debugging
     */
    static bool debuggingFlag;
};


#endif //STAGE_FIGTHER_BULLETUNIVERSE_H
