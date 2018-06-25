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
public:
    struct Animation {
        std::string name;
        float startTime;
        float endTime;
        bool loop;

        Animation() {};
        Animation(const std::string &name, float startTime, float endTime, bool loop) :
                name(name), startTime(startTime), endTime(endTime), loop(loop) {};
    };

private:
    unsigned int instanceID = 1;
    int instances = 0;
    bool recomputeInstanceBuffer = false;

    std::map<unsigned long, size_t> instanceMapping;
    std::vector<glm::mat4> instancedModelMatrix;
    std::vector<glm::mat4> instancedNormalMatrix;
    std::vector<glm::vec3> instancedTranslation;
    std::vector<glm::quat> instancedRotation;

    glm::mat4 nodeMatrix;

    unsigned long gltfNodeIndex;
    std::vector<GLuint> vbos;
    std::map<std::shared_ptr<Shader>, GLuint> VAOs;
    GLenum meshDrawMode;
    GLuint modelMatrixInstanceVBO;
    GLuint normalMatrixInstanceVBO;

    struct InternalAnimationData {
        tinygltf::AnimationSampler translation ;//= nullptr;
        tinygltf::AnimationSampler rotation    ;//= nullptr;
        float currentAnimationTime              = 0.0f;
        bool inAnimation                        = false;
    };

    Animation animationData;
    InternalAnimationData animDataInternal;

    std::shared_ptr<tinygltf::Model> gltfModel;
    std::vector<std::shared_ptr<Texture>> textures;

    void prepareModelMatrices();
    void prepareModelMatrices(const unsigned long pos);

    void drawNode(const tinygltf::Node &node, std::shared_ptr<Shader> &shader);
    void drawMesh(const tinygltf::Mesh &mesh, GLuint &VAO, GLenum &mode, std::shared_ptr<Shader> &shader);

    glm::mat4 getNodeMatrix();

    glm::vec3 getAnimationTranslation();
    glm::quat getAnimationRotation();

public:
    /**
     * Create a Renderable gltf Model Object
     * @param model gltf model
     * @param shader shader which should be used
     * @param instances 0 = no instances, >0 instances are used
     */
    Model3DObject(const glm::vec3 &position, float bsRadius, const std::shared_ptr<tinygltf::Model> &model,
                  const std::vector<std::shared_ptr<Shader>> &shader, int instances = 0);
    ~Model3DObject();

    std::vector<std::shared_ptr<Texture>> &getTextures() { return this->textures; }

    //void render(Scene *scene) override;

    void draw(std::shared_ptr<Shader> &shader) override;

    // Instancing not fully implemented !
    unsigned int addInstance(const glm::vec3 &vec, const glm::quat &rot);
    void setInstance(unsigned int  id, const glm::vec3 &vec, const glm::quat &rot);
    void removeInstance(unsigned int id);

    void setOrigin(const glm::vec3 &vec) override;
    void setRotation(const glm::quat &rot);

    void updateModelMatrix() { this->prepareModelMatrices(); }
    void clearInstances();

    void setModelMatrix(const glm::mat4 &mat) override;

    void enableAnimation(const Animation &);
    void disableAnimation();
    void applyAnimation(float currentTime);

    GLuint getInstancedModelVBO() const { return this->normalMatrixInstanceVBO; }
};


#endif //STAGE_FIGHTER_MODEL3DOBJECT_H
