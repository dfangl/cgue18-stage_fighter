//
// Created by raphael on 13.03.18.
//

#ifndef STAGE_FIGTHER_OBJECT3D_H
#define STAGE_FIGTHER_OBJECT3D_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../Shader.h"

class Object3D {

protected:
    GLuint VBO, VAO, EBO;
    std::shared_ptr<Shader> shader;

    glm::mat4 transform;

public:
    explicit Object3D(const std::shared_ptr<Shader> &shader);

    virtual void render();
    virtual void draw() = 0;

    virtual void rotate(float angle, const glm::vec3 &vec);
    virtual void translate(const glm::vec3 &vec);

};


#endif //STAGE_FIGTHER_OBJECT3D_H
