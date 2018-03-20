//
// Created by raphael on 19.03.18.
//

#ifndef STAGE_FIGHTER_MODEL3DOBJECT_H
#define STAGE_FIGHTER_MODEL3DOBJECT_H

#include <vector>
#include <tiny_gltf.h>

#include "Object3D.h"
#include "../Texture.h"
#include "../helper/Logger.h"




class Model3DObject : public Object3D, Logger {

private:
    std::vector<GLuint> vbos;
    std::shared_ptr<tinygltf::Model> gltfModel;
    std::shared_ptr<Texture> texture0;

    void drawNode(const tinygltf::Node &node);
    void drawMesh(const tinygltf::Mesh &mesh);

public:
    Model3DObject(const std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<Shader> &shader);

    void draw() override;

};


#endif //STAGE_FIGHTER_MODEL3DOBJECT_H
