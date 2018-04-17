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

#include "../helper/Logger.h"

#include "../Camera.h"
#include "../Window.h"

#include "../manager/TextureManager.h"
#include "../manager/ModelManager.h"

#include "../controller/CameraController.h"

#include "../entity/Entity.h"
#include "../entity/CubeEntity.h"
#include "../entity/Player.h"

#include "../object3d/Model3DObject.h"

#include "LuaClassWrapper.h"

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

    LuaVec3 cameraPos {0.0f, 0.0f, 0.0f};
    LuaVec3 lookAt {0.0f, 0.0f, 0.0f};

    std::shared_ptr<Player> player;
    int playerInputCallbackID;

    const std::shared_ptr<BulletUniverse> world;
    Window *window;

    bool paused = false;
    LevelState levelState = PAUSED;

public:
    Level(const std::string &file);

    void start(Camera &camera, Window *window);
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
};


#endif //STAGE_FIGHTER_LEVEL_H
