//
// Created by raphael on 19.03.18.
//

#ifndef STAGE_FIGHTER_MODEL3DOBJECT_H
#define STAGE_FIGHTER_MODEL3DOBJECT_H

#include <vector>
#include <map>
#include <memory>

#include <glad/glad.h>

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

    unsigned int instanceID = 1;
    int instances = 0;
    bool recomputeInstanceBuffer = false;

    std::vector<std::vector<glm::mat4>> instancedModelMatrix;
    std::vector<std::vector<glm::mat4>> instancedNormalMatrix;

    std::vector<GLuint> vbos;
    GLuint *vaos;
    GLenum *meshDrawMode;
    GLuint *modelMatrixInstanceVBO;
    GLuint *normalMatrixInstanceVBO;

    struct Matrix {
        unsigned int id;

        std::vector<glm::mat4> modelMatrix;
        std::vector<glm::mat4> normalMatrix;

        glm::vec3 translation;
        glm::quat rotation;

        /* don't use*/ Matrix() : id(0) {};
        Matrix(unsigned  int id, glm::vec3 translation, glm::quat rotation) :
                id(id),
                translation(translation),
                rotation(rotation) {}
    };

    std::shared_ptr<tinygltf::Model> gltfModel;
    std::vector<std::shared_ptr<Texture>> textures;
    std::map<unsigned  int, Matrix> modelMatrix;

    void prepareModelMatrices();
    void prepareModelMatrices(Matrix &instance);

    void drawNode(const int idx, const tinygltf::Node &node, Matrix &instance);
    void drawMesh(const tinygltf::Mesh &mesh, GLuint &VAO, GLenum &mode);

public:
    /**
     * Create a Renderable gltf Model Object
     * @param model gltf model
     * @param shader shader which should be used
     * @param instances 0 = no instances, >0 instances are used
     */
    Model3DObject(const std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<Shader> &shader, int instances = 0);
    ~Model3DObject();

    std::vector<std::shared_ptr<Texture>> &getTextures() { return this->textures; }

    void draw() override;

    unsigned int addInstance(const glm::vec3 &vec, const glm::quat &rot);
    void setInstance(unsigned int  id, const glm::vec3 &vec, const glm::quat &rot);
    void removeInstance(unsigned int id);

    void setOrigin(const glm::vec3 &vec) override;
    void setRotation(const glm::quat &rot);

    void updateModelMatrix() { this->prepareModelMatrices(); }
    void clearInstances();
};


#endif //STAGE_FIGHTER_MODEL3DOBJECT_H
