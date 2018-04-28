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
#include "../Scene.h"
#include "../Camera.h"

#include "Light.h"

/**
 * This Interface represents all Objects which can be drawn with a Camera into something
 */
class Object3DAbstract {
public:
    virtual void render(Scene *scene) = 0;
};

/**
 * Object3D represnets a Object in OpenGL which can use VBO, VAO, EBO and do have a Model matrix
 */
class Object3D : public Object3DAbstract {

protected:
    GLuint VBO = 0, VAO = 0, EBO = 0;
    std::shared_ptr<Shader> shader;

    glm::mat4 model = glm::mat4(1.0f);

public:
    explicit Object3D(const std::shared_ptr<Shader> &shader);

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

};


#endif //STAGE_FIGTHER_OBJECT3D_H
