//
// Created by raphael on 13.03.18.
//

#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string vertexCode, const std::string fragmentCode) : Logger("Shader"){
    /*
     * Convert from C++ string to C string
     */
    GLuint vertex, fragment;
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    /*
     * It's needed fo some error handling
     */
    int success;
    char infoLog[512];

    /*
     * Compile Vertex and Fragment Shaders:
     */
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        logger->error("Vertex Shader compilation failed:\n{}", infoLog);
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
        logger->error("Fragment Shader compilation failed:\n{}", infoLog);
    }

    /*
     * Link the Shader and Attach the fragment and vertex shader code
     */
    this->shaderID = glCreateProgram();
    glAttachShader(this->shaderID, vertex);
    glAttachShader(this->shaderID, fragment);
    glLinkProgram(this->shaderID);

    glGetProgramiv(this->shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->shaderID, 512, nullptr, infoLog);
        logger->error("Shader linking failed:\n{}", infoLog);
    }

    /*
     * Vertex and Fragment Shader Codes can be deleted since they are in the Program
     */
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {
    glDeleteProgram(this->shaderID);
}

void Shader::use() {
    glUseProgram(this->shaderID);
}

void Shader::setUniform(const GLint location, bool value) {
    glUniform1i(location, (int)value);
}

void Shader::setUniform(const GLint location, int value) {
    glUniform1i(location, value);
}

void Shader::setUniform(const GLint location, float value) {
    glUniform1f(location, value);
}

void Shader::setUniform(const GLint location, const glm::vec2 &value) {
    glUniform2fv(location, 1, &value[0]);
}

void Shader::setUniform(const GLint location, const glm::vec3 &value) {
    glUniform3fv(location, 1, &value[0]);
}

void Shader::setUniform(const GLint location, const glm::vec4 &value) {
    glUniform4fv(location, 1, &value[0]);
}

void Shader::setUniform(const GLint location, const glm::mat2 &mat) {
    glUniformMatrix2fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::setUniform(const GLint location, const glm::mat3 &mat) {
    glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::setUniform(const GLint location, const glm::mat4 &mat) {
    glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

std::shared_ptr<Shader> Shader::fromFile(const std::string vertex, const std::string fragment) {
    auto console = spdlog::get("console");

    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertex);
        fShaderFile.open(fragment);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure& e) {
        console->error("Unable to load shader files: {}, {}", vertex, fragment);
        console->error("Error: {}", e.what());
        console->flush();

        throw e;
    }

    return std::make_shared<Shader>(vertexCode, fragmentCode);
}

GLint Shader::getLocation(const std::string &name) {
    auto element = this->mapping.find(name);
    if(element == this->mapping.end()) {
        const GLint location = glGetUniformLocation(this->shaderID, name.c_str());
        this->mapping[name] = location;

        return location;
    }

    return element->second;
}

void Shader::setVertexAttributePointer(const GLuint location, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) {
    glVertexAttribPointer(location, size, type, normalized, stride, pointer);
    glEnableVertexAttribArray(location);
}

void Shader::setVertexAttributePointer(const std::string &name, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer) {
    if(this->mapping.find(name) == this->mapping.end()) {
        const GLint location = glGetAttribLocation(this->shaderID, name.c_str());
        if(location<0)
            return;

        this->mapping[name] = location;
    }

    setVertexAttributePointer(static_cast<const GLuint>(this->mapping[name]), size, type, normalized, stride, pointer);
}

void Shader::disableVAO(const GLuint location) {
    glDisableVertexAttribArray(location);
}

void Shader::disableVAO(const std::string &name) {
    auto element = this->mapping.find(name);
    if(element == this->mapping.end()) {
        return;
    }

    glDisableVertexAttribArray(static_cast<GLuint>(element->second));
}
