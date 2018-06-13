//
// Created by raphael on 13.03.18.
//

#ifndef STAGE_FIGTHER_OBJECT3D_H
#define STAGE_FIGTHER_OBJECT3D_H

#include <vector>
#include <memory>

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "../Shader.h"
class Scene; //#include "../Scene.h"
#include "../Camera.h"

#include "Light.h"

// Must be the same as in the shaders!
#define MAX_LIGHTS ((size_t)30L)

/**
 * This Interface represents all Objects which can be drawn with a Camera into something
 */
class Object3DAbstract {

public:
    virtual void render(Scene *scene) = 0;

    virtual float getBoundingSphereRadius() = 0;
    virtual const glm::vec3 &getPosition() const = 0;
};

/**
 * Object3D represnets a Object in OpenGL which can use VBO, VAO, EBO and do have a Model matrix
 */
class Object3D : virtual public Object3DAbstract {

protected:
    GLuint VBO = 0, VAO = 0;//, EBO = 0;
    std::shared_ptr<Shader> shader;

    glm::vec3 position;
    glm::mat4 model = glm::mat4(1.0f);

    float boundingSphereRadius;

public:
    explicit Object3D(const glm::vec3 &position, float boundingSphereRadius, const std::shared_ptr<Shader> &shader);

    /**
     * This function does apply all the matries to the shader
     * @param camera
     */
    void render(Scene *scene) override;

    /**
     * This function should implement the drawing logic of the Object
     */
    virtual void draw() = 0;

    virtual void rotate(float angle, const glm::vec3 &vec);
    virtual void rotate(const glm::quat &quat);

    virtual void translate(const glm::vec3 &vec);
    virtual void setOrigin(const glm::vec3 &vec);

    void setModelMatrix(const glm::mat4 &mat);
    const glm::mat4 &getModelMatrix() const;

    float getBoundingSphereRadius() override { return this->boundingSphereRadius; }
    const glm::vec3 &getPosition() const override { return this->position; }

};


#endif //STAGE_FIGTHER_OBJECT3D_H
