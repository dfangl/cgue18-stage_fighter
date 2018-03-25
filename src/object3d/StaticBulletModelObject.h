//
// Created by raphael on 22.03.18.
//

#ifndef STAGE_FIGHTER_BULLETMODELOBJECT_H
#define STAGE_FIGHTER_BULLETMODELOBJECT_H

#include <vector>
#include <btBulletDynamicsCommon.h>

#include "BulletObject.h"
#include "Model3DObject.h"

class StaticBulletModelObject : public BulletObject, public Model3DObject {


private:
    struct btVertexData {
        btVector3 v1;
        btVector3 v2;
        btVector3 v3;
    };

    std::vector<BulletObject> additionalObjects;
    btCollisionObject *collisionObject;


protected:
    static btBvhTriangleMeshShape *convertModelToTriangleMesh(const std::shared_ptr<tinygltf::Model> &model);

public:
    StaticBulletModelObject(const btVector3 &pos, const btQuaternion &rotation, btScalar mass,
                            const std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<Shader> &shader);


    btCollisionObject *getStaticObject() { return this->collisionObject; }

};


#endif //STAGE_FIGHTER_BULLETMODELOBJECT_H
