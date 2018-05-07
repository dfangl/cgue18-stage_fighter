//
// Created by raphael on 19.03.18.
//

#ifndef STAGE_FIGHTER_MODEL3DOBJECT_H
#define STAGE_FIGHTER_MODEL3DOBJECT_H

#include <vector>
#include <map>
#include <memory>

#include <tiny_gltf.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Object3D.h"
#include "../Texture.h"
#include "../helper/Logger.h"

/**
 * With this class a GLTF Model can be drawn in OpenGL
 */
class Model3DObject : public Object3D, Logger {

private:
    std::vector<GLuint> vbos;

    std::shared_ptr<tinygltf::Model> gltfModel;
    std::vector<std::shared_ptr<Texture>> textures;
    std::map<int, glm::mat4> modelMatrix;
    std::map<int, glm::mat4> normalMatrix;

    glm::vec3 translation;
    glm::quat rotation;

    void prepareModelMatrices();
    void drawNode(const int idx, const tinygltf::Node &node);
    void drawMesh(const tinygltf::Mesh &mesh);

public:
    Model3DObject(const std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<Shader> &shader);

    std::vector<std::shared_ptr<Texture>> &getTextures() { return this->textures; }

    void draw() override;

    void setOrigin(const glm::vec3 &vec) override;
    void setRotation(const glm::quat &rot);

    void updateModelMatrix() { this->prepareModelMatrices(); }
};


#endif //STAGE_FIGHTER_MODEL3DOBJECT_H
