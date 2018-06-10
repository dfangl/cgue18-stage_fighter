//
// Created by Raphael on 28.04.2018.
//

#include "object3d/Object3D.h"
#include "bullet/GLDebugDrawer.h"

#include "Scene.h"

#include <glm/glm.hpp>

Scene::Scene(Camera &camera) : camera(camera) {}

void Scene::render(std::chrono::duration<double, std::milli> &delta) {
    this->deltaT = delta;
    this->culledObjects = 0;

    {
        auto error = glGetError();
        if(error != GL_NO_ERROR) {
            spdlog::get("console")->error("[Scene Entry] OpenGL Error Code: {}", error);
        }
    }


    if (dirtyLights) {
        glBindBuffer(GL_ARRAY_BUFFER, this->lightVBO);
        {
            auto error = glGetError();
            if(error != GL_NO_ERROR) {
                spdlog::get("console")->error("[lightVBO] OpenGL Error Code: {}", error);
            }
        }

        glBufferData(GL_ARRAY_BUFFER, lights.size() * sizeof(Light), lights.data(), GL_STATIC_DRAW);

        {
            auto error = glGetError();
            if(error != GL_NO_ERROR) {
                spdlog::get("console")->error("[lightVBO] OpenGL Error Code: {}", error);
            }
        }
                glBindBuffer(GL_ARRAY_BUFFER, 0);

        {
            auto error = glGetError();
            if(error != GL_NO_ERROR) {
                spdlog::get("console")->error("[lightVBO] OpenGL Error Code: {}", error);
            }
        }
    }

    for (auto &obj : this->objects) {

        if (frustumCulling &&
            isSphereInFrustum(obj->getPosition(), obj->getBoundingSphereRadius()) == Camera::OUTSIDE) {
            this->culledObjects++;
            continue;

            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            //obj->render(this);
            //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
            obj->render(this);
        }

        {
            auto error = glGetError();
            if(error != GL_NO_ERROR) {
                spdlog::get("console")->error("[obj] OpenGL Error Code: {}", error);
            }
        }
    }

    // Draw Bullet Debug Context after all Objects, just in case ...
    if (this->bulletDebugDrawer)
        this->bulletDebugDrawer->render(this);

    // Draw skybox as last element to make use of the depth buffer
    if (this->skybox)
        this->skybox->render(this);

    // Since particles do not use the DEPTH buffer the skybox would
    // overdraw them, so we have to draw them after the skybox
    for (auto &obj : this->particles) {
        if (frustumCulling &&
            isSphereInFrustum(obj->getPosition(), obj->getBoundingSphereRadius()) == Camera::OUTSIDE) {
            this->culledObjects++;
            continue;
        } else {
            obj->render(this);
        }

        {
            auto error = glGetError();
            if(error != GL_NO_ERROR) {
                spdlog::get("console")->error("[particle system] OpenGL Error Code: {}", error);
            }
        }
    }

    dirtyLights = false;
}

Camera::FrustumLocation Scene::isSphereInFrustum(const glm::vec3 &position, float radius) {
    return camera.isInFrustum(position, radius);
/*

    auto result = Camera::INSIDE;
    auto pl = camera.getFrustumPlanes();

    for (int i=0; i<6; i++) {
        //(pl[i].d + glm::dot(pl[i].normal, position));
        auto distance = glm::dot(pl[i], glm::vec4(position.x, position.y, position.z, 1));

        //if (distance < -radius)
        //    spdlog::get("console")->info("plane:{}, distance={}, ({}+{}) position={},{},{}", i, distance, pl[i].d, glm::dot(pl[i].normal, position), position.x,position.y,position.z);

        if (distance < -radius) return Camera::OUTSIDE;
        else if (distance < radius)
            result = Camera::INTERSECT;
    }

    return result;
*/
}

void Scene::addObject(const std::shared_ptr<Object3DAbstract> &object3D) {
    this->objects.push_back(object3D);
}

void Scene::removeObject(const std::shared_ptr<Object3DAbstract> &object3D) {
    this->objects.erase(
            std::remove_if(
                    this->objects.begin(),
                    this->objects.end(),
                    [object3D](std::shared_ptr<Object3DAbstract> current) -> bool {
                        return current == object3D;
                    }
            ),
            this->objects.end()
    );
}

void Scene::addLight(const Light &light) {
    this->dirtyLights = true;
    this->lights.push_back(light);
}

void Scene::removeLight(const Light &light) {
    this->dirtyLights = true;
    this->lights.erase(
            std::remove_if(
                    this->lights.begin(),
                    this->lights.end(),
                    [&light](Light &current) -> bool {
                        return current.position == light.position;
                    }
            ),
            this->lights.end()
    );
}

std::vector<Light> &Scene::getLights() {
    return this->lights;
}

Camera &Scene::getCamera() {
    return this->camera;
}

void Scene::clear() {
    this->lights.clear();
    this->objects.clear();
}

void Scene::addParticleSystem(const std::shared_ptr<ParticleSystem> &s) {
    this->particles.push_back(s);
}

void Scene::removeParticleSystem(const std::shared_ptr<ParticleSystem> &s) {
    this->particles.erase(
            std::remove_if(
                    this->particles.begin(),
                    this->particles.end(),
                    [s](std::shared_ptr<ParticleSystem> current) -> bool {
                        return current == s;
                    }
            ),
            this->particles.end()
    );
}

void Scene::initOpenGLContext() {
    glGenBuffers(1, &this->lightVBO);

    {
        auto error = glGetError();
        if(error != GL_NO_ERROR) {
            spdlog::get("console")->error("[oglInit] OpenGL Error Code: {}", error);
        }
    }
}
