//
// Created by Raphael on 28.04.2018.
//

#ifndef STAGE_FIGHTER_SCENE_H
#define STAGE_FIGHTER_SCENE_H

#include "Camera.h"

#include "object3d/Light.h"
#include "object3d/Skybox.h"
#include "object3d/ParticleSystem.h"

#include "manager/TextureManager.h"

class Object3DAbstract;
class GLDebugDrawer;

class Scene {

private:
    Camera &camera;

    std::vector<Light> lights;
    std::vector<std::shared_ptr<Object3DAbstract>> objects;
    std::vector<std::shared_ptr<AbstractParticleSystem>> particles;

    std::shared_ptr<Skybox> skybox;

    std::chrono::duration<double, std::milli> deltaT;

    GLuint lightSSBO  = 0;
    bool dirtyLights = false;

protected:
    Camera::FrustumLocation isSphereInFrustum(const glm::vec3 &position, float radius);


public:
    unsigned int culledObjects = 0;
    explicit Scene(Camera &camera);

    void initOpenGLContext();

    void render(std::chrono::duration<double, std::milli> &delta);

    void addObject(const std::shared_ptr<Object3DAbstract> &object3D);
    void removeObject(const std::shared_ptr<Object3DAbstract> &object3D);

    void addLight(const Light &light);
    void removeLight(const Light &light);

    void addParticleSystem(const std::shared_ptr<AbstractParticleSystem> &);
    void removeParticleSystem(const std::shared_ptr<AbstractParticleSystem> &);

    void clear();

    void setSkybox(const std::shared_ptr<Skybox> &skybox) { this->skybox = skybox; }
    void removeSkybox() { this->skybox = nullptr; }

    std::shared_ptr<GLDebugDrawer> bulletDebugDrawer;

    std::vector<Light> &getLights();
    Camera &getCamera();

    float gamma = 1.0f;

    bool frustumCulling = true;
    unsigned int getCulledObjectCount() const { return this->culledObjects; }

    std::chrono::duration<double, std::milli> &getFrameTime() { return this->deltaT; }

    bool areLightsDirty() const { return this->dirtyLights; }
    void makeLightsDirty() { this->dirtyLights = true; }

    GLuint getLightVBO() const { return this->lightSSBO; }
};

#endif //STAGE_FIGHTER_SCENE_H
