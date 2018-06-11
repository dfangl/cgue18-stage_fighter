//
// Created by Raphael on 28.03.2018.
//

#ifndef STAGE_FIGHTER_LEVEL_H
#define STAGE_FIGHTER_LEVEL_H

#include <chrono>
#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <kaguya/kaguya.hpp>

#include <btBulletDynamicsCommon.h>
#include <glm/gtc/type_ptr.hpp>
#include <kaguya/state.hpp>

#include "../helper/Logger.h"

#include "../Camera.h"
#include "../Window.h"

#include "../manager/TextureManager.h"
#include "../manager/ModelManager.h"

#include "../controller/CameraController.h"

#include "../entity/Entity.h"
#include "../entity/Player.h"

#include "../widget/Label.h"

class Light;
#include "../object3d/Model3DObject.h"

#include "LuaClassWrapper.h"
#include "../entity/InstancedProjectile.h"

class Level : public Logger {

public:
    enum LevelState {
        PLAYING, LOST, WON, PAUSED
    };

private:
    kaguya::State state;

    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<std::shared_ptr<Entity>> newEntities;
    std::vector<Entity *> oldEntities;
    std::vector<std::shared_ptr<Model3DObject>> statics;
    std::vector<std::shared_ptr<BulletObject>> bullet;
    std::vector<Light> lights;
    std::vector<std::shared_ptr<InstancedProjectile>> projectiles;

    std::shared_ptr<Player> player;
    std::shared_ptr<Object3DAbstract> playerWitcheryPointer;
    int playerInputCallbackID;

    std::shared_ptr<BulletUniverse> world;
    Window *window;

    LevelState levelState = PAUSED;

    std::shared_ptr<Label> winLoseLabel;
    std::shared_ptr<Label> loadingStatusLabel;

protected:
    void setLabel(const std::string text);
    void setLoadingStatus(std::string thing, int cur, int max);

public:
    explicit Level(const std::string &file);
    ~Level();

    void start(Window *window);
    void tick(std::chrono::duration<double, std::milli> delta);

    void destroy();
    void resetEnvironment();

    void hide();
    void show();

    void pause();
    void resume();

    LevelState getLevelState() const { return this->levelState; }

    void spawn(std::shared_ptr<Entity> entity);
    void despawn(Entity *entity);

    std::shared_ptr<Player> getPlayer() const { return this->player; }
    std::shared_ptr<BulletUniverse> getWorld() const { return this->world; }

    // Lua Binding part:
    void luaSpawnEntity(const int projectile, const LuaVec3 &spawn, const LuaVec3 &target);
    LuaVec3 luaGetPlayerPos();

};

#endif //STAGE_FIGHTER_LEVEL_H
