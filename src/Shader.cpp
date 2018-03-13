//
// Created by raphael on 13.03.18.
//

#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string vertexCode, const std::string fragmentCode) {
    GLuint vertex, fragment;
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    //TODO: Check for errors
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    //TODO: Check for errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    this->shaderID = glCreateProgram();
    glAttachShader(this->shaderID, vertex);
    glAttachShader(this->shaderID, fragment);
    glLinkProgram(this->shaderID);
    // check for linking errors
    glGetProgramiv(this->shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(this->shaderID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {
    glDeleteProgram(this->shaderID);
}

void Shader::use() {
    glUseProgram(this->shaderID);
}

void Shader::setUniform(const GLint location, bool value) const {
    glUniform1i(location, (int)value);
}

void Shader::setUniform(const GLint location, int value) const {
    glUniform1i(location, value);
}

void Shader::setUniform(const GLint location, float value) const {
    glUniform1f(location, value);
}

void Shader::setUniform(const GLint location, const glm::vec2 &value) const {
    glUniform2fv(location, 1, &value[0]);
}

void Shader::setUniform(const GLint location, const glm::vec3 &value) const {
    glUniform3fv(location, 1, &value[0]);
}

void Shader::setUniform(const GLint location, const glm::vec4 &value) const {
    glUniform4fv(location, 1, &value[0]);
}

void Shader::setUniform(const GLint location, const glm::mat2 &mat) const {
    glUniformMatrix2fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::setUniform(const GLint location, const glm::mat3 &mat) const {
    glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
}

void Shader::setUniform(const GLint location, const glm::mat4 &mat) const {
    glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
}

std::shared_ptr<Shader> Shader::fromFile(const std::string vertex, const std::string fragment) {
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
        throw e;
    }

    return std::make_shared<Shader>(vertexCode, fragmentCode);
}
