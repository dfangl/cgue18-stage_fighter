//
// Created by raphael on 24.05.18.
//

#ifndef STAGE_FIGHTER_SCRIPTEDENTITY_H
#define STAGE_FIGHTER_SCRIPTEDENTITY_H

#include <kaguya/kaguya.hpp>

#include "../object3d/BulletObject.h"
#include "../object3d/Model3DObject.h"

#include "../BulletUniverse.h"

#include "Entity.h"
#include "AbstractScriptedObject.h"

class ScriptedEntity : public Entity, virtual public AbstractScriptedObject, public BulletObject, public Model3DObject {

private:
    std::shared_ptr<BulletUniverse> world;
    glm::vec3 collisionOffset;

public:
    ScriptedEntity(const std::string &name, int health, const btVector3 &pos, float bsRadius, const btQuaternion &rot, const std::string &model, float mass,
    glm::vec3 collisionOffset, btCollisionShape *hitbox, const std::shared_ptr<BulletUniverse> &world, kaguya::LuaTable scriptEnv);
    ~ScriptedEntity() override;

    void think(Level *level, std::chrono::duration<double, std::milli> delta) override;

    void collideWith(BulletObject *other) override;

    void setPosition(const glm::vec3 &vec, const glm::quat &rot) override;

    void think(std::chrono::duration<double, std::milli> delta) override;

    void render(Scene *scene) override;

    float getBoundingSphereRadius() override;

    const glm::vec3 &getPosition() const override;

    Kind getEntityKind() override;
};

#endif //STAGE_FIGHTER_SCRIPTEDENTITY_H
