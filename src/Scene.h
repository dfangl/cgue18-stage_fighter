//
// Created by Raphael on 28.04.2018.
//

#ifndef STAGE_FIGHTER_SCENE_H
#define STAGE_FIGHTER_SCENE_H

#include "Camera.h"
#include "object3d/Light.h"
#include "object3d/Skybox.h"

class Object3DAbstract;
class GLDebugDrawer;

class Scene {

private:
    Camera &camera;

    std::vector<std::shared_ptr<Light>> lights;
    std::vector<std::shared_ptr<Object3DAbstract>> objects;

    std::shared_ptr<Skybox> skybox;

public:
    explicit Scene(Camera &camera);

    void render();

    void addObject(const std::shared_ptr<Object3DAbstract> &object3D);
    void removeObject(const std::shared_ptr<Object3DAbstract> &object3D);

    void addLight(const std::shared_ptr<Light> &light);
    void removeLight(const std::shared_ptr<Light> &light);

    void clear();

    void setSkybox(std::shared_ptr<Skybox> &skybox) { this->skybox = skybox; }

    std::shared_ptr<GLDebugDrawer> bulletDebugDrawer;

    std::vector<std::shared_ptr<Light>> &getLights();
    Camera &getCamera();

    float gamma = 1.0f;
};

#endif //STAGE_FIGHTER_SCENE_H
