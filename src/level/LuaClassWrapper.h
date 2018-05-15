//
// Created by Raphael on 29.03.2018.
//

#ifndef STAGE_FIGHTER_LUACLASSWRAPPER_H
#define STAGE_FIGHTER_LUACLASSWRAPPER_H

#include <glm/glm.hpp>
#include <btBulletCollisionCommon.h>

#include "../object3d/BulletObject.h"

#include "../entity/Entity.h"
#include "../entity/EnemyEntity.h"
#include "../entity/BulletEntity.h"

// ********************
// ==== Glm Vectors ===
// ********************

class LuaVec3 {
public:
    glm::vec3 pos;

    LuaVec3(const LuaVec3 &source) = default;
    LuaVec3(double x, double y, double z) : pos(x,y,z) {}

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
            position(pos), rotation(rot), mass(mass) {}

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

public:
    LuaStaticObject(const std::string model, const std::string shader, const LuaVec3 position, const kaguya::LuaTable bullet)
            : position(position), model(model), shader(shader), bullet(bullet), rotation(LuaVec4(0,0,0,1)) {}
    LuaStaticObject(const std::string model, const std::string shader, const LuaVec3 position, const LuaVec4 rotation, const kaguya::LuaTable bullet)
            : position(position), model(model), shader(shader), bullet(bullet), rotation(rotation) {}

    std::shared_ptr<Model3DObject> toModel() const {
        auto ret = std::make_shared<Model3DObject>(ModelManager::load(model), ShaderManager::load(shader), 0);
        ret->setRotation(glm::quat(rotation.pos.w, rotation.pos.x, rotation.pos.y, rotation.pos.z));
        ret->setOrigin(position.pos);
        ret->updateModelMatrix();

        if (ret->getTextures().size() > 1)
            ret->getTextures()[1] = TextureManager::load("__gen_marble");

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

class LuaCubeEntity : public LuaEntity {
protected:
    const std::string texture;

public:
    LuaCubeEntity(const std::string texture, const LuaVec3 position) : LuaEntity(position), texture(texture) {}

    std::shared_ptr<Entity> toEntity3D(const std::shared_ptr<BulletUniverse> &world) const override {
        return std::make_shared<CubeEntity>(position.pos, TextureManager::load(texture), world);
    }
};

class LuaEnemyEntity : public LuaEntity {

protected:
    std::string name;
    int health;
    int spawnTime;
    LuaVec4 rot;
    std::string model;
    float mass;
    btCollisionShape *hitbox;
    LuaVec3 hitBoxOffset;

public:
    LuaEnemyEntity(std::string name, int health, int spawnTime, const LuaVec3 &pos, const LuaVec4 &rot, std::string model, float mass, const LuaBtCollisionShape &hitbox) :
            LuaEntity(pos), name(name), health(health), spawnTime(spawnTime), rot(rot), model(model), mass(mass), hitBoxOffset(hitbox.position) {
        this->hitbox = hitbox.generateShape();
    }

    std::shared_ptr<Entity> toEntity3D(const std::shared_ptr<BulletUniverse> &world) const override {
        return std::make_shared<EnemyEntity>(name, health, spawnTime, position.toVector3(), rot.toQuat(), model, mass, hitBoxOffset.pos, hitbox, world);
    }

};

class LuaLight {
protected:
    LuaVec3 position;
    LuaVec3 diffuse;
    LuaVec3 specular;
    LuaVec3 ambient;
    float power;

public:
    LuaLight(LuaVec3 &pos,LuaVec3 &dif,LuaVec3 &spec,LuaVec3 &amb, float power) :
            position(pos), diffuse(dif), specular(spec), ambient(amb), power(power) {}

    std::shared_ptr<Light> toLight() {
        return std::make_shared<Light>(
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
    /* don't use */ LuaProjectile() : model("__NAN__"), hitbox(nullptr) {};
    LuaProjectile(std::string model, float speed, float mass, const LuaBtCollisionShape &hitbox) : model(model), speed(speed) {
        this->hitbox = hitbox.generateShape();
    }
};

#endif //STAGE_FIGHTER_LUACLASSWRAPPER_H
