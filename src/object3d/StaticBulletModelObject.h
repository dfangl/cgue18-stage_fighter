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

/**
 * This class can construct a Bullet Shape from a given 3D Model, but it doesn't work for now, so don't use it until
 * it is magically fixed ...
 */
class StaticBulletModelObject : public BulletObject, public Model3DObject {

private:
    std::shared_ptr<BulletUniverse> world;

protected:
    static btBvhTriangleMeshShape *convertModelToTriangleMesh(const std::shared_ptr<tinygltf::Model> &model);
    static btQuaternion getModelRotationQuat(const std::shared_ptr<tinygltf::Model> &model);

public:
    struct StaticModel {
        std::shared_ptr<tinygltf::Model> graphical;
        std::shared_ptr<tinygltf::Model> collision;
    };

    StaticBulletModelObject(const btVector3 &pos, const StaticModel &model, const std::shared_ptr<Shader> &shader,
                            std::shared_ptr<BulletUniverse> &world);

};


#endif //STAGE_FIGHTER_BULLETMODELOBJECT_H
