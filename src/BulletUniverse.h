//
// Created by raphael on 13.03.18.
//

#ifndef STAGE_FIGTHER_BULLETUNIVERSE_H
#define STAGE_FIGTHER_BULLETUNIVERSE_H

#include <chrono>

#include <btBulletDynamicsCommon.h>

class BulletUniverse {

private:
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

public:
    explicit BulletUniverse(const btVector3 &gravity);
    ~BulletUniverse();

    void addRigidBody(btRigidBody*body);
    void removeRigidBody(btRigidBody*body);

    void simulate(std::chrono::duration<double, std::milli> tick);

};


#endif //STAGE_FIGTHER_BULLETUNIVERSE_H
