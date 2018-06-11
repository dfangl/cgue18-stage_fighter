//
// Created by Raphael on 29.03.2018.
//

#ifndef STAGE_FIGHTER_LUACLASSWRAPPER_H
#define STAGE_FIGHTER_LUACLASSWRAPPER_H

#include <glm/glm.hpp>
#include <btBulletCollisionCommon.h>

#include "../object3d/BulletObject.h"

#include "../entity/Entity.h"
#include "../entity/BulletEntity.h"
#include "../entity/ScriptedEntity.h"

#include "../manager/ModelManager.h"
#include "../manager/TextureManager.h"

#include "../helper/CompilerMacros.h"

// ********************
// ==== Glm Vectors ===
// ********************

class LuaVec3 {
public:
    glm::vec3 pos;

    LuaVec3(const LuaVec3 &source) = default;
    LuaVec3(double x, double y, double z) : pos(x,y,z) {}
    LuaVec3(glm::vec3 &source) : pos(source) {}

    LuaVec3 &operator=(const LuaVec3 &source) = default;

    btVector3 toVector3() const {
        return {pos.x, pos.y, pos.z};
    }

    double x() { return pos.x; }
    double y() { return pos.y; }
    double z() { return pos.z; }
};

class LuaVec4 {
public:
    glm::vec4 pos;

    LuaVec4(const LuaVec4 &source) : pos(source.pos) {};
    LuaVec4(double x, double y, double z, double w) : pos(x,y,z,w) {}

    LuaVec4 &operator=(const LuaVec4 &source) {
        this->pos = source.pos;
        return *this;
    };

    btQuaternion toQuat() const {
        return {pos.x, pos.y, pos.z, pos.w};
    }

    double x() { return pos.x; }
    double y() { return pos.y; }
    double z() { return pos.z; }
    double w() { return pos.w; }
};

// *******************************
// === Bullet Collision Shapes ===
// *******************************

class LuaBtCollisionShape {
protected:
    const double mass;

public:
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
private:
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
        auto ret = std::make_shared<Model3DObject>(position.pos, bsRadius, ModelManager::load(model), ShaderManager::load(shader), 0);
        ret->setRotation(glm::quat(rotation.pos.w, rotation.pos.x, rotation.pos.y, rotation.pos.z));
        //ret->setOrigin(position.pos);
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

class LuaEntity{
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
                position.pos,
                diffuse.pos,
                specular.pos,
                ambient.pos,
                power
        );
    }

};

class LuaProjectile {
protected:
    std::string model;
    btCollisionShape *hitbox;
    float speed;
    float mass;

public:
    LuaProjectile(std::string model, float speed, float UNUSED(mass), const LuaBtCollisionShape &hitbox) :
            model(model), speed(speed) {
        this->hitbox = hitbox.generateShape();
    }

    std::shared_ptr<BulletEntity> toBulletEntity(const LuaVec3 &pos, const LuaVec3 &target, std::shared_ptr<BulletUniverse> &world) const {
        return std::make_shared<BulletEntity>(pos.toVector3(), target.toVector3(), world);
    }
};

class LuaScriptedEntity : public LuaEntity {

protected:
    const std::string name;
    const int health;
    const LuaVec4 rot;
    const std::string model;
    const float mass;
    btCollisionShape *hitbox;
    const LuaVec3 hitBoxOffset;
    const kaguya::LuaTable env;
    const float bsRadius;

public:
    LuaScriptedEntity(std::string name, int health, const LuaVec3 &pos, float bsRadius, const LuaVec4 &rot, std::string model,
                      float mass, const LuaBtCollisionShape &hitbox, kaguya::LuaTable env) :
    LuaEntity(pos), name(name), health(health), rot(rot), model(model), mass(mass), hitBoxOffset(hitbox.position),
    env(env), bsRadius(bsRadius) {
        this->hitbox = hitbox.generateShape();
    }

    std::shared_ptr<Entity> toEntity3D(const std::shared_ptr<BulletUniverse> &world) const override {
        return std::make_shared<ScriptedEntity>(name, health, position.toVector3(), bsRadius, rot.toQuat(), model, mass, hitBoxOffset.pos, hitbox, world, env);
    }

};

#endif //STAGE_FIGHTER_LUACLASSWRAPPER_H
