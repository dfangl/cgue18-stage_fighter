//
// Created by Raphael on 13.06.2018.
//

#ifndef STAGE_FIGHTER_SCRIPTEDOBJECT_H
#define STAGE_FIGHTER_SCRIPTEDOBJECT_H

#include <chrono>

#include <kaguya/kaguya.hpp>

#include "AbstractScriptedObject.h"

#include "../object3d/BulletObject.h"
#include "../object3d/Model3DObject.h"

#include "../BulletUniverse.h"
#include "../object3d/particlesystem/ScriptedParticleSystem.h"

class Level;
class Scene;

class ScriptedObject : virtual public AbstractScriptedObject, public BulletObject, public Model3DObject {

private:
    const std::shared_ptr<BulletUniverse> world;
    std::vector<std::shared_ptr<ScriptedParticleSystem>> particleSystem;
    std::vector<glm::vec3> offsets;

    bool stickyToPlayer;

public:
    ScriptedObject(const glm::vec3 &pos, const glm::quat &rotation, btCollisionShape *, btScalar mass,
                   float bsRadius, const std::shared_ptr<tinygltf::Model> &model, const std::vector<std::shared_ptr<Shader>> &shader,
                   int instances, kaguya::LuaTable env, const std::shared_ptr<BulletUniverse> &world);
    ~ScriptedObject();

    void think(Level *level, std::chrono::duration<double, std::milli> delta);
    void update(Scene *scene);

    void show(Scene *);
    void hide(Scene *);

    bool isSticky() const { return this->stickyToPlayer; }
    const btVector3 &getLinearVelocity() const { return this->rigidBody->getLinearVelocity(); }

};


#endif //STAGE_FIGHTER_SCRIPTEDOBJECT_H
