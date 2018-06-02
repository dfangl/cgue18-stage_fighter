//
// Created by Raphael on 28.04.2018.
//

#ifndef STAGE_FIGHTER_SCENE_H
#define STAGE_FIGHTER_SCENE_H

#include "Camera.h"
#include "object3d/Light.h"
#include "object3d/Skybox.h"
#include "object3d/Cube.h"
#include "manager/TextureManager.h"

class Object3DAbstract;
class GLDebugDrawer;

class Scene {

private:
    Camera &camera;

    unsigned int culledObjects = 0;

    std::vector<std::shared_ptr<Light>> lights;
    std::vector<std::shared_ptr<Object3DAbstract>> objects;

    std::shared_ptr<Skybox> skybox;

    std::chrono::duration<double, std::milli> deltaT;

    GLuint vbo,vao;
    std::vector<std::shared_ptr<Cube>> vfDc;

protected:
    Camera::FrustumLocation isSphereInFrustum(const glm::vec3 &position, float radius);


public:
    explicit Scene(Camera &camera);

    void render(std::chrono::duration<double, std::milli> &delta);

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

    bool frustumCulling = true;
    unsigned int getCulledObjectCount() const { return this->culledObjects; }

    std::vector<std::shared_ptr<Object3DAbstract>> lastStageRender;

    void initFustrumDEBUG() {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3[2]), nullptr);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);

        vfDc.push_back(std::make_shared<Cube>(glm::vec3(0,0,0), TextureManager::load("__gen_marble")));
        vfDc.push_back(std::make_shared<Cube>(glm::vec3(0,0,0), TextureManager::load("__gen_marble")));
        vfDc.push_back(std::make_shared<Cube>(glm::vec3(0,0,0), TextureManager::load("__gen_marble")));
        vfDc.push_back(std::make_shared<Cube>(glm::vec3(0,0,0), TextureManager::load("__gen_marble")));
        vfDc.push_back(std::make_shared<Cube>(glm::vec3(0,0,0), TextureManager::load("__gen_marble")));
            vfDc.push_back(std::make_shared<Cube>(glm::vec3(0,0,0), TextureManager::load("__gen_marble")));
            vfDc.push_back(std::make_shared<Cube>(glm::vec3(0,0,0), TextureManager::load("__gen_marble")));
            vfDc.push_back(std::make_shared<Cube>(glm::vec3(0,0,0), TextureManager::load("__gen_marble")));
            vfDc.push_back(std::make_shared<Cube>(glm::vec3(0,0,0), TextureManager::load("__gen_marble")));
    }

    std::chrono::duration<double, std::milli> &getFrameTime() { return this->deltaT; }
};

#endif //STAGE_FIGHTER_SCENE_H
