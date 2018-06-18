//
// Created by Raphael on 29.03.2018.
//

#ifndef STAGE_FIGHTER_LUACLASSWRAPPER_H
#define STAGE_FIGHTER_LUACLASSWRAPPER_H

#include <glm/glm.hpp>
#include <btBulletCollisionCommon.h>

#include "../object3d/BulletObject.h"

#include "../entity/Entity.h"
#include "../entity/ScriptedEntity.h"

#include "../manager/ModelManager.h"
#include "../manager/TextureManager.h"

#include "../helper/CompilerMacros.h"
#include "../object3d/particlesystem/ScriptedParticleSystem.h"
#include "../entity/InstancedProjectile.h"
#include "../entity/ScriptedObject.h"
#include "../object3d/Trigger.h"

// ********************
// ==== Glm Vectors ===
// ********************

class LuaVec3 {
public:
    glm::vec3 vec3;

    LuaVec3(const LuaVec3 &source) = default;
    LuaVec3(double x, double y, double z) : vec3(x,y,z) {}
    LuaVec3(glm::vec3 &source) : vec3(source) {}

    LuaVec3 &operator=(const LuaVec3 &source) = default;

    btVector3 toVector3() const {
        return {vec3.x, vec3.y, vec3.z};
    }

    double x() { return vec3.x; }
    double y() { return vec3.y; }
    double z() { return vec3.z; }
};

class LuaVec4 {
public:
    glm::vec4 vec4;

    LuaVec4(const LuaVec4 &source) : vec4(source.vec4) {};
    LuaVec4(double x, double y, double z, double w) : vec4(x,y,z,w) {}

    LuaVec4 &operator=(const LuaVec4 &source) {
        this->vec4 = source.vec4;
        return *this;
    };

    btQuaternion toQuat() const {
        return {vec4.x, vec4.y, vec4.z, vec4.w};
    }

    glm::quat toGlmQuat() const {
        return {vec4.w, vec4.x, vec4.y, vec4.z};
    }

    btVector4 toVector4() const {
        return btVector4(vec4.x, vec4.y, vec4.z, vec4.w);
    }

    double x() { return vec4.x; }
    double y() { return vec4.y; }
    double z() { return vec4.z; }
    double w() { return vec4.w; }
};

// *******************************
// === Bullet Collision Shapes ===
// *******************************

class LuaBtCollisionShape {
public:
    const double mass;
    const LuaVec3 position;
    const LuaVec4 rotation;

    LuaBtCollisionShape(LuaVec3 const &pos, LuaVec4 const &rot, const double mass) :
            mass(mass), position(pos), rotation(rot) {}

    virtual btCollisionShape *generateShape() const = 0;

    std::shared_ptr<BulletObject> toCollisionShape() const {
        return std::make_shared<BulletObject>(position.toVector3(), rotation.toQuat(), generateShape(), mass);
    }
};

class LuaBtSphere : public LuaBtCollisionShape {
private:
    const double radius;

public:
    LuaBtSphere(LuaVec3 const &position, LuaVec4 const &rotation, double mass, double radius) :
            LuaBtCollisionShape(position, rotation, mass), radius(radius) {}

    btCollisionShape *generateShape() const override {
        return new btSphereShape((btScalar) radius);
    }
};

class LuaBtBox : public LuaBtCollisionShape {
private:
    const LuaVec3 size;

public:
    LuaBtBox(LuaVec3 const &pos, LuaVec4 const &rot, const double mass, LuaVec3 const &size) :
            LuaBtCollisionShape(pos, rot, mass), size(size) {}

    btCollisionShape *generateShape() const override {
        return new btBoxShape(size.toVector3());
    }
};

// **********************
// === Static Objects ===
// **********************

class LuaStaticObject {
protected:
    const std::string model, shader;
    const LuaVec3 position;
    const LuaVec4 rotation;
    const kaguya::LuaTable bullet;
    const kaguya::LuaTable texOverride;
    const float bsRadius;

public:
    LuaStaticObject(const std::string model, const std::string shader, const LuaVec3 position, float bsRadius,
                    const LuaVec4 rotation, const kaguya::LuaTable bullet, kaguya::LuaTable texO)
            :  model(model), shader(shader), position(position),  rotation(rotation), bullet(bullet), texOverride(texO),
                bsRadius(bsRadius) {}

    std::shared_ptr<Model3DObject> toModel() const {

        auto ret = std::make_shared<Model3DObject>(position.vec3, bsRadius, ModelManager::load(model), std::vector<std::shared_ptr<Shader>> {ShaderManager::load(shader)}, 0);
        ret->setRotation(rotation.toGlmQuat());
        ret->updateModelMatrix();

        if (texOverride.size() > 0 )
            for (auto &texture : texOverride.map<unsigned int, std::string>()) {
                if (ret->getTextures().size() > texture.first) {
                    ret->getTextures()[texture.first] = TextureManager::load(texture.second);
                } else {
                    spdlog::get("console")->warn("Texture will be appended and not overridden (array to small)!");
                    ret->getTextures().push_back(TextureManager::load(texture.second));
                }
            }

        return ret;
    }

    std::vector<std::shared_ptr<BulletObject>> collisions() const {
        std::vector<std::shared_ptr<BulletObject>> data;

        for (auto &b : bullet.map<int, LuaBtCollisionShape *>()) {
            data.push_back(b.second->toCollisionShape());
        }

        return data;
    }
};

// ******************
// ==== Entities ====
// ******************

class LuaEntity {
protected:
    const LuaVec3 position;

public:
    explicit LuaEntity(const LuaVec3 pos) : position(pos) {}

    virtual std::shared_ptr<Entity> toEntity3D(const std::shared_ptr<BulletUniverse> &world) const = 0;
};

class LuaLight {
protected:
    const LuaVec3 position;
    const LuaVec3 diffuse;
    const LuaVec3 specular;
    const LuaVec3 ambient;
    const float power;

public:
    LuaLight(LuaVec3 &pos,LuaVec3 &dif,LuaVec3 &spec,LuaVec3 &amb, float power) :
            position(pos), diffuse(dif), specular(spec), ambient(amb), power(power) {}

    Light const toLight() {
        return Light(
                position.vec3,
                diffuse.vec3,
                specular.vec3,
                ambient.vec3,
                power
        );
    }

};

class LuaScriptedParticleSystem {

protected:
    const float bsRadius;
    const std::string shader;
    const std::string texture;
    const kaguya::LuaTable env;

public:
    LuaScriptedParticleSystem(float radius, const std::string shader, const std::string texture, kaguya::LuaTable scriptEnv)
            : bsRadius(radius), shader(shader), texture(texture), env(scriptEnv) {}

    std::shared_ptr<ScriptedParticleSystem> toParticleSystem(const glm::vec3 &position, unsigned int particles, bool generate = false) const {
        auto tmp = std::make_shared<ScriptedParticleSystem>(
                position, bsRadius, std::vector<std::shared_ptr<Shader>> { ShaderManager::load(shader, true)}, TextureManager::load(texture),
                particles, env

        );

        if (generate)
            tmp->init();

        return tmp;
    }

};

class LuaProjectile {
protected:
    const std::string model;
    const std::string shader;
    const LuaVec3 hitbox;
    const float speed;
    const float radius;
    const float mass;
    const LuaScriptedParticleSystem ps;

public:
    LuaProjectile(std::string model, std::string shader, float speed, float radius, float mass, const LuaVec3 &hitbox, LuaScriptedParticleSystem &ps) :
            model(model), shader(shader), hitbox(hitbox), speed(speed), radius(radius), mass(mass), ps(ps) {
    }

    std::shared_ptr<InstancedProjectile> toProjectile(std::shared_ptr<BulletUniverse> &world) const {
        return std::make_shared<InstancedProjectile>(
                radius, ModelManager::load(model), std::vector<std::shared_ptr<Shader>> {ShaderManager::load(shader)}, hitbox.toVector3(), mass,
                world, &ps
        );
    }
};

class LuaScriptedEntity : public LuaEntity {

protected:
    const std::string name;
    const int health;
    const LuaVec4 rot;
    const std::string model;
    const float mass;
    const LuaBtBox hitbox;
    const LuaVec3 hitBoxOffset;
    const kaguya::LuaTable env;
    const float bsRadius;

public:
    LuaScriptedEntity(std::string name, int health, const LuaVec3 &pos, float bsRadius, const LuaVec4 &rot, std::string model,
                      float mass, const LuaBtBox &hitbox, kaguya::LuaTable env) :
    LuaEntity(pos), name(name), health(health), rot(rot), model(model), mass(mass), hitbox(hitbox), hitBoxOffset(hitbox.position),
    env(env), bsRadius(bsRadius) {}

    std::shared_ptr<Entity> toEntity3D(const std::shared_ptr<BulletUniverse> &world) const override {
        return std::make_shared<ScriptedEntity>(name, health, position.toVector3(), bsRadius, rot.toQuat(), model, mass, hitBoxOffset.vec3, hitbox.generateShape(), world, env);
    }

};

class LuaScriptedObject {

protected:
    const std::string model, shader;
    const LuaVec3 position;
    const LuaVec4 rotation;
    const LuaBtBox bullet;
    const kaguya::LuaTable texOverride;
    const float bsRadius;
    const int instances;
    const kaguya::LuaTable env;
    const double mass;

public:
    LuaScriptedObject(const std::string model, const std::string shader, const LuaVec3 &position, float bsRadius,
                      const LuaVec4 &rotation, const LuaBtBox &bullet, const kaguya::LuaTable &texO, int i, const kaguya::LuaTable &env)
    :  model(model), shader(shader), position(position),  rotation(rotation), bullet(bullet), texOverride(texO),
       bsRadius(bsRadius), instances(i), env(env), mass(bullet.mass) {}

    std::shared_ptr<ScriptedObject> toScriptedObject(const std::shared_ptr<BulletUniverse> &world) const {
        auto ret = std::make_shared<ScriptedObject>(position.vec3, rotation.toGlmQuat(), bullet.generateShape(), mass, bsRadius,
                                                    ModelManager::load(model), std::vector<std::shared_ptr<Shader>> {ShaderManager::load(shader)}, instances,
                                                    env, world
        );

        if (texOverride.size() > 0 )
            for (auto &texture : texOverride.map<unsigned int, std::string>()) {
                if (ret->getTextures().size() > texture.first) {
                    ret->getTextures()[texture.first] = TextureManager::load(texture.second);
                } else {
                    spdlog::get("console")->warn("Texture will be appended and not overridden (array to small)!");
                    ret->getTextures().push_back(TextureManager::load(texture.second));
                }
            }

        return ret;
    }

};

class LuaTrigger {

protected:
    const LuaVec3 position;
    const float radius;
    const kaguya::LuaTable env;

public:
    LuaTrigger(const LuaVec3 &position, const float radius, kaguya::LuaTable table)
            : position(position), radius(radius), env(table) {}

    std::shared_ptr<Trigger> toTrigger() {
        return std::make_shared<Trigger>(position.vec3, radius, env);
    }

};


#endif //STAGE_FIGHTER_LUACLASSWRAPPER_H
