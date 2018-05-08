//
// Created by raphael on 13.03.18.
//

#ifndef STAGE_FIGTHER_SHADER_H
#define STAGE_FIGTHER_SHADER_H

#include <string>
#include <memory>
#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "helper/Logger.h"

/**
 * This Class does read the .vert and .frag Files from Disk and does load the Shader to the GPU Memory, additionally
 * it ceeps a mapping between names and positions in the Shader so that setUniform("name", ...) can be called without
 * querieng the position each frame
 */
class Shader : public Logger {

private:
    // ID of the Shader in OpenGL
    GLuint shaderID;

    // Mapping of uniform locations and their names
    std::map<std::string, GLint> mapping;

    bool loadedFromFile = false;
    std::string vertexFilePath;
    std::string fragmentFilePath;

    struct Code {
        std::string vertex;
        std::string fragment;
    };

    GLuint compile(const std::string &vertexCode,const std::string &fragmentCode);
    static Code loadFromFile(const std::string &vertex,const std::string &fragment);

public:
    /*
     * Create a Shader from a Vertex and a Fragment shader code
     */
    Shader(const std::string vertexCode,const std::string fragmentCode);

    /*
     * Load a Shader from a vertex and a fragment shader file
     */
    static std::shared_ptr<Shader> fromFile(const std::string vertex,const std::string fragment);

    ~Shader();

    /*
     * Enable the Shader for use in OpenGL
     */
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

    void setVertexAttribDivisor(const GLuint location, const GLuint divisor);
    void setVertexAttribDivisor(const GLuint location, const GLuint count, const GLuint divisor);
    void setVertexAttribDivisor(const std::string &name, const GLuint divisor);
    void setVertexAttribDivisor(const std::string &name, const GLuint count, const GLuint divisor);

    void disableVAO(const GLuint location);
    void disableVAO(const std::string &name);

    void recompile();
};


#endif //STAGE_FIGTHER_SHADER_H
