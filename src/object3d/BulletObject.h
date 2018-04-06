//
// Created by Raphael on 17.03.2018.
//

#ifndef STAGE_FIGHTER_BULLETOBJECT_H
#define STAGE_FIGHTER_BULLETOBJECT_H

#include <btBulletDynamicsCommon.h>
#include "Object3D.h"

class BulletObject {
public:
    enum Kind {
        PLAYER, ENEMY, BULLET, ENVIRONMENT, WEAPON
    };

protected:
    btDefaultMotionState* motionState;
    btRigidBody* rigidBody;
    btCollisionShape* fallShape;
    btScalar mass;

    Kind kind = ENVIRONMENT;

public:
    explicit BulletObject(const btVector3 &pos, const btQuaternion &rotation, btCollisionShape* bulletShape, btScalar mass = 0.001);
    virtual ~BulletObject();

    virtual void setOrigin(const btVector3 &pos, const btQuaternion &rotation = btQuaternion(0,0,0,1));

    btRigidBody *getRigidBody() { return this->rigidBody; }
    btTransform getTransformation();

    virtual void collideWith(BulletObject *other) {};

    Kind getKind() const { return this->kind; }
};


#endif //STAGE_FIGHTER_BULLETOBJECT_H
