//
// Created by raphael on 22.03.18.
//

#ifndef STAGE_FIGHTER_BULLETMODELOBJECT_H
#define STAGE_FIGHTER_BULLETMODELOBJECT_H

#include <memory>
#include <vector>
#include <btBulletDynamicsCommon.h>

#include "BulletObject.h"
#include "Model3DObject.h"
#include "../BulletUniverse.h"

class StaticBulletModelObject : public BulletObject, public Model3DObject {

private:
    std::vector<BulletObject> additionalObjects;
    std::shared_ptr<BulletUniverse> world;

protected:
    static btBvhTriangleMeshShape *convertModelToTriangleMesh(const std::shared_ptr<tinygltf::Model> &model);

public:
    StaticBulletModelObject(const btVector3 &pos, const btQuaternion &rotation, btScalar mass,
                            const std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<Shader> &shader,
                            std::shared_ptr<BulletUniverse> &world);

};


#endif //STAGE_FIGHTER_BULLETMODELOBJECT_H
