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
#include "../entity/Entity.h"
#include "../Camera.h"
#include "../Window.h"
#include "../entity/CubeEntity.h"
#include "../manager/TextureManager.h"
#include "../object3d/Model3DObject.h"
#include "../manager/ModelManager.h"
#include "LuaClassWrapper.h"
#include "../controller/CameraController.h"

class Level : public Logger {

private:
    kaguya::State state;

    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<std::shared_ptr<Model3DObject>> statics;
    std::vector<std::shared_ptr<BulletObject>> bullet;

    LuaVec3 cameraPos {0.0f, 0.0f, 0.0f};
    LuaVec3 lookAt {0.0f, 0.0f, 0.0f};

    std::shared_ptr<CameraEntity> player;
    int playerInputCallbackID;

    const std::shared_ptr<BulletUniverse> world;
    Window *window;

public:
    Level(const std::string &file, const std::shared_ptr<BulletUniverse> &world);

    void start(Camera &camera, Window *window);
    void tick(std::chrono::duration<double, std::milli> delta);

    void destroy();
    void resetEnvironment();

    void hide();
    void show();
};


#endif //STAGE_FIGHTER_LEVEL_H
