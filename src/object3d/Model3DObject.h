//
// Created by raphael on 19.03.18.
//

#ifndef STAGE_FIGHTER_MODEL3DOBJECT_H
#define STAGE_FIGHTER_MODEL3DOBJECT_H

#include <vector>
#include <map>
#include <memory>
#include <glm/glm.hpp>
#include <tiny_gltf.h>

#include "Object3D.h"
#include "../Texture.h"
#include "../helper/Logger.h"

class Model3DObject : public Object3D, Logger {

private:
    std::vector<GLuint> vbos;

    std::shared_ptr<tinygltf::Model> gltfModel;
    std::vector<std::shared_ptr<Texture>> textures;
    std::map<int, glm::mat4> modelMatrix;

    glm::vec3 translation;
    //glm::quat rotation;

    void prepareModelMatrices();
    void drawNode(const int idx, const tinygltf::Node &node);
    void drawMesh(const tinygltf::Mesh &mesh);

public:
    Model3DObject(const std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<Shader> &shader);

    void draw() override;

    void setOrigin(const glm::vec3 &vec) override;
};


#endif //STAGE_FIGHTER_MODEL3DOBJECT_H
