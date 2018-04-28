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
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

    bool debug = false;
    std::shared_ptr<GLDebugDrawer> debugDrawer;
    double internalTick = 0.0;

public:
    explicit BulletUniverse(const btVector3 &gravity);
    ~BulletUniverse();

    void addRigidBody(btRigidBody *body);
    void removeRigidBody(btRigidBody *body);

    void addCollsionObject(btCollisionObject *body);
    void removeCollsipnObject(btCollisionObject *body);

    /**
     * Simulate the Bullet World, tick is the time in ms which passed since the last
     * call of this function
     *
     * @param tick Delta T
     */
    void simulate(std::chrono::duration<double, std::milli> tick);
    /**
     * DO NOT USE THIS FUNCTION IN THE GAME LOOP!
     *  (Only exists to step into the simulation so that first step isn't invalid)
     *
     * @param d some delta
     */
    void __simulate_fixed_step__(float d);

    std::shared_ptr<GLDebugDrawer> getDebugDrawer() { return this->debugDrawer; }
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
