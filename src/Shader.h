//
// Created by raphael on 13.03.18.
//

#ifndef STAGE_FIGTHER_SHADER_H
#define STAGE_FIGTHER_SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>

class Shader {

private:
    GLuint shaderID;

public:
    Shader(const std::string vertexCode,const std::string fragmentCode);
    static std::shared_ptr<Shader> fromFile(const std::string vertex,const std::string fragment);

    ~Shader();

    void use();

    template<typename T>
    void setUniform(const std::string &name, T value) const {
        this->setUniform(glGetUniformLocation(this->shaderID, name.c_str()), value);
    }

    void setUniform(const GLint location, bool value) const;
    void setUniform(const GLint location, int value) const;
    void setUniform(const GLint location, float value) const;
    void setUniform(const GLint location, const glm::vec2 &value) const;
    void setUniform(const GLint location, const glm::vec3 &value) const;
    void setUniform(const GLint location, const glm::vec4 &value) const;
    void setUniform(const GLint location, const glm::mat2 &mat) const;
    void setUniform(const GLint location, const glm::mat3 &mat) const;
    void setUniform(const GLint location, const glm::mat4 &mat) const;
};


#endif //STAGE_FIGTHER_SHADER_H
