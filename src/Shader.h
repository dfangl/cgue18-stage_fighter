//
// Created by raphael on 13.03.18.
//

#ifndef STAGE_FIGTHER_SHADER_H
#define STAGE_FIGTHER_SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <map>
#include "helper/Logger.h"


class Shader : public Logger {

private:
    GLuint shaderID;
    std::map<std::string, GLint> mapping;

public:
    Shader(const std::string vertexCode,const std::string fragmentCode);
    static std::shared_ptr<Shader> fromFile(const std::string vertex,const std::string fragment);

    ~Shader();

    void use();

    GLint getLocation(const std::string &name);

    template<typename T>
    void setUniform(const std::string &name, T value) {
        this->setUniform(getLocation(name), value);
    }

    template<typename T>
    void setUniformIfNeeded(const std::string &name, T value) {
        GLint location = getLocation(name);

        if (location >= 0)
            this->setUniform(location, value);
    }

    void setUniform(const GLint location, bool value);
    void setUniform(const GLint location, int value);
    void setUniform(const GLint location, float value);
    void setUniform(const GLint location, const glm::vec2 &value);
    void setUniform(const GLint location, const glm::vec3 &value);
    void setUniform(const GLint location, const glm::vec4 &value);
    void setUniform(const GLint location, const glm::mat2 &mat);
    void setUniform(const GLint location, const glm::mat3 &mat);
    void setUniform(const GLint location, const glm::mat4 &mat);

    void setVertexAttributePointer(const GLuint location, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
    void setVertexAttributePointer(const std::string &name, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);

    void disableVAO(const GLuint location);
    void disableVAO(const std::string &name);
};


#endif //STAGE_FIGTHER_SHADER_H
