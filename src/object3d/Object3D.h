//
// Created by raphael on 13.03.18.
//

#ifndef STAGE_FIGTHER_OBJECT3D_H
#define STAGE_FIGTHER_OBJECT3D_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../Shader.h"
#include "../Camera.h"

class Object3DAbstract {
public:
    virtual void render(const Camera &camera) = 0;
};

class Object3D : public Object3DAbstract {

protected:
    GLuint VBO = 0, VAO = 0, EBO = 0;
    std::shared_ptr<Shader> shader;

    glm::mat4 model = glm::mat4(1.0f);

public:
    explicit Object3D(const std::shared_ptr<Shader> &shader);

    virtual void render(const Camera &camera);
    virtual void draw() = 0;

    virtual void rotate(float angle, const glm::vec3 &vec);
    virtual void rotate(const glm::quat &quat);

    virtual void translate(const glm::vec3 &vec);
    virtual void setOrigin(const glm::vec3 &vec);

    glm::mat4 getModelMatrix() const {
        return this->model;
    }

};


#endif //STAGE_FIGTHER_OBJECT3D_H
