//
// Created by Raphael on 28.04.2018.
//

#include "object3d/Object3D.h"
#include "bullet/GLDebugDrawer.h"

#include "Scene.h"

Scene::Scene(Camera &camera) : camera(camera) {}

void Scene::render() {
    for (auto &obj : this->objects)
        obj->render(this);

    // Draw Bullet Debug Context after all Objects, just in case ...
    if (this->bulletDebugDrawer)
        this->bulletDebugDrawer->render(this);

    // Draw skybox as last element to make use of the depth buffer
    if (this->skybox)
        this->skybox->render(this);
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
