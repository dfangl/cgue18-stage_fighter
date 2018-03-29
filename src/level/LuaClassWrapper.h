//
// Created by Raphael on 29.03.2018.
//

#ifndef STAGE_FIGHTER_LUACLASSWRAPPER_H
#define STAGE_FIGHTER_LUACLASSWRAPPER_H

#include <glm/glm.hpp>
#include <btBulletCollisionCommon.h>

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
};

class LuaVec4 {
public:
    glm::vec4 pos;

    LuaVec4(const LuaVec4 &source) : pos(source.pos) {
        spdlog::get("console")->info("LuaVec4 {} copy constr from {}", (void *)this, (void *)&source);
        spdlog::get("console")->info("LuaVec4 {}: {}, {}, {}, {}", (void *)this, pos.x, pos.y, pos.z, pos.w);
    };
    LuaVec4(double x, double y, double z, double w) : pos(x,y,z,w) {
        spdlog::get("console")->info("LuaVec4 {}: {}, {}, {}, {}", (void *)this, x, y, z, w);
    }

    LuaVec4 &operator=(const LuaVec4 &source) {
        this->pos = source.pos;
        spdlog::get("console")->info("LuaVec4 {} operator= {}", (void *)this, (void *)&source);
        spdlog::get("console")->info("LuaVec4 {}: {}, {}, {}, {}", (void *)this, pos.x, pos.y, pos.z, pos.w);

        return *this;
    };

    btQuaternion toQuat() const {
        return {pos.x, pos.y, pos.z, pos.w};
    }
};

// *******************************
// === Bullet Collision Shapes ===
// *******************************

class LuaBtCollisionShape {
protected:
    const LuaVec3 position;
    const LuaVec4 rotation;
    const double mass;

    virtual btCollisionShape *generateShape() const = 0;

public:
    LuaBtCollisionShape(LuaVec3 const &pos, LuaVec4 const &rot, const double mass) :
            position(pos), rotation(rot), mass(mass) {
        spdlog::get("console")->info("LuaBtCollisionShape {}: rotation:{}, rot:{}", (void *)this, (void *)&pos, (void *)&rot);
        spdlog::get("console")->info("LuaBtCollisionShape {}: {},{},{},{} ({})", (void *)this, rot.pos.x, rot.pos.y, rot.pos.z, rot.pos.w, (void *)&rot);
    }

    std::shared_ptr<BulletObject> toCollisionShape() const {
        spdlog::get("console")->info("btCollisionShape {}: {},{},{}, {},{},{},{}, shape, {}", (void *)this, position.pos.x, position.pos.y, position.pos.z, rotation.pos.x, rotation.pos.y, rotation.pos.z, rotation.pos.w, mass);
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
        spdlog::get("console")->info("btBoxShape: {}, {}, {}", size.pos.x, size.pos.y, size.pos.z);
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
    const kaguya::LuaTable bullet;

public:
    LuaStaticObject(const std::string model, const std::string shader, const LuaVec3 position, const kaguya::LuaTable bullet)
            : position(position), model(model), shader(shader), bullet(bullet) {}

    std::shared_ptr<Model3DObject> toModel() const {
        auto ret = std::make_shared<Model3DObject>(ModelManager::load(model), ShaderManager::load(shader));
        ret->setOrigin(position.pos);

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

#endif //STAGE_FIGHTER_LUACLASSWRAPPER_H
