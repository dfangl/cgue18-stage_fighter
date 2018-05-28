//
// Created by Raphael on 28.04.2018.
//

#include "object3d/Object3D.h"
#include "bullet/GLDebugDrawer.h"

#include "Scene.h"

#include <glm/glm.hpp>

Scene::Scene(Camera &camera) : camera(camera) {}

void Scene::render() {
    culledObjects = 0;

    for (auto &obj : this->objects) {

        if (frustumCulling && this->isSphereInFrustum(obj->getPosition(), obj->getBoundingSphereRadius()) == Camera::OUTSIDE) {
            this->culledObjects++;
            continue;

            //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            //obj->render(this);
            //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
            obj->render(this);
        }
    }

    // Draw Bullet Debug Context after all Objects, just in case ...
    if (this->bulletDebugDrawer)
        this->bulletDebugDrawer->render(this);

    // Draw skybox as last element to make use of the depth buffer
    if (this->skybox)
        this->skybox->render(this);

    /*
    if (!vfDc.empty()) {
        for (int i=4; i<8; i++)
            vfDc[i]->setOrigin(camera.debug_frustumPlanes[i]);

        for (int i=4; i<8; i++)
            vfDc[i]->render(this);

        ShaderManager::load("light")->use();
        glDisable(GL_CULL_FACE);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * camera.debug_frustumPlanes.size(),
                     camera.debug_frustumPlanes.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(camera.debug_frustumPlanes.size()));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(vao);
    }
     */
}

Camera::FrustumLocation Scene::isSphereInFrustum(const glm::vec3 &position, float radius) {
//    return camera.isInFrustum(position, radius);


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

void Scene::addLight(const std::shared_ptr<Light> &light) {
    this->lights.push_back(light);
}

void Scene::removeLight(const std::shared_ptr<Light> &light) {
    this->lights.erase(
            std::remove_if(
                    this->lights.begin(),
                    this->lights.end(),
                    [light](std::shared_ptr<Light> current) -> bool {
                        return current == light;
                    }
            ),
            this->lights.end()
    );
}

std::vector<std::shared_ptr<Light>> &Scene::getLights() {
    return this->lights;
}

Camera &Scene::getCamera() {
    return this->camera;
}

void Scene::clear() {
    this->lights.clear();
    this->objects.clear();
}
