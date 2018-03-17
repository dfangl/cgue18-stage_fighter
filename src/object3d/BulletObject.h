//
// Created by Raphael on 17.03.2018.
//

#ifndef STAGE_FIGHTER_ENTITY_H
#define STAGE_FIGHTER_ENTITY_H

#include <btBulletDynamicsCommon.h>
#include "Object3D.h"

class BulletObject {

private:
    btDefaultMotionState* motionState;
    btRigidBody* rigidBody;
    btCollisionShape* fallShape;
    btScalar mass;

public:
    explicit BulletObject(const btVector3 &pos, const btQuaternion &rotation, btCollisionShape* bulletShape, btScalar mass = 0.001);
    virtual ~BulletObject();

    virtual void setOrigin(const btVector3 &pos, const btQuaternion &rotation = btQuaternion(0,0,0,1));

    btRigidBody *getRigidBody() { return this->rigidBody; }
    btTransform getTransformation();
};


#endif //STAGE_FIGHTER_ENTITY_H
