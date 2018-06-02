//
// Created by raphael on 13.03.18.
//

#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>

#define GL_LOG_SIZE (512)

Shader::Shader(const std::string vertexCode, const std::string fragmentCode) : Logger("Shader"){
    this->shaderID = compile(vertexCode, fragmentCode);
}

Shader::Shader(const std::string computeCode) {
    this->shaderID = compileComputeShader(computeCode);
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
    Shader::Code code = Shader::loadFromFile(vertex, fragment);

    auto shader = std::make_shared<Shader>(code.vertex, code.fragment);
    shader->loadedFromFile = true;
    shader->vertexFilePath = vertex;
    shader->fragmentFilePath = fragment;

    return shader;
}

std::shared_ptr<Shader> Shader::fromFile(const std::string compute) {
    Shader::Code code = Shader::loadFromFile(compute);

    auto shader = std::make_shared<Shader>(code.compute);
    shader->loadedFromFile = true;
    shader->computeFilePath = compute;

    return shader;
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
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, size, type, normalized, stride, pointer);
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

GLuint Shader::compile(const std::string &vertexCode, const std::string &fragmentCode) {
    /*
        * Convert from C++ string to C string
        */
    GLuint vertex, fragment, shaderID;
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    /*
     * It's needed fo some error handling
     */
    int success;
    char infoLog[GL_LOG_SIZE];

    /*
     * Compile Vertex and Fragment Shaders:
     */
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, GL_LOG_SIZE, nullptr, infoLog);
        logger->error("Vertex Shader compilation failed:\n{}", infoLog);
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, GL_LOG_SIZE, nullptr, infoLog);
        logger->error("Fragment Shader compilation failed:\n{}", infoLog);
    }

    /*
     * Link the Shader and Attach the fragment and vertex shader code
     */
    shaderID = glCreateProgram();
    glAttachShader(shaderID, vertex);
    glAttachShader(shaderID, fragment);
    glLinkProgram(shaderID);

    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderID, GL_LOG_SIZE, nullptr, infoLog);
        logger->error("Shader linking failed:\n{}", infoLog);
    }

    /*
     * Vertex and Fragment Shader Codes can be deleted since they are in the Program
     */
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return shaderID;
}

void Shader::recompile() {
    if (!loadedFromFile) {
        logger->error("Unable to recompile Shader which is not loaded from a File!");
        return;
    }

    GLuint oldShader;
    if (this->computeFilePath.empty()) {
        Shader::Code code = Shader::loadFromFile(this->vertexFilePath, this->fragmentFilePath);
        auto newShader = this->compile(code.vertex, code.fragment);

        // Swap Shader IDs before deletion
        oldShader = this->shaderID;
        this->shaderID = newShader;
    } else {
        Shader::Code code = Shader::loadFromFile(this->computeFilePath);
        auto newShader = this->compileComputeShader(code.compute);

        // Swap Shader IDs before deletion
        oldShader = this->shaderID;
        this->shaderID = newShader;
    }

    glDeleteShader(oldShader);
}

Shader::Code Shader::loadFromFile(const std::string &vertex, const std::string &fragment) {
    auto console = spdlog::get("console");

    Shader::Code code;
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

        code.vertex = vShaderStream.str();
        code.fragment = fShaderStream.str();
    } catch (std::ifstream::failure& e) {
        console->error("Unable to load shader files: {}, {}", vertex, fragment);
        console->error("Error: {}", e.what());
        console->flush();

        throw e;
    }

    return code;
}

Shader::Code Shader::loadFromFile(const std::string &compute) {
    auto console = spdlog::get("console");

    Shader::Code code;
    std::ifstream vComputeFile;

    vComputeFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vComputeFile.open(compute);
        std::stringstream vComputeStream;

        vComputeStream << vComputeFile.rdbuf();
        vComputeFile.close();

        code.compute = vComputeStream.str();
    } catch (std::ifstream::failure &e) {
        console->error("Unable to load shader files {}, {}", compute);
        console->error("Error: {}", e.what());
        console->flush();

        throw e;
    }

    return code;
}


void Shader::setVertexAttribDivisor(const GLuint location, const GLuint divisor) {
    glVertexAttribDivisor(location, divisor);
}

void Shader::setVertexAttribDivisor(const GLuint location, const GLuint count, const GLuint divisor) {
    for (int i=0; i<count; i++)
        glVertexAttribDivisor(location + i, divisor);
}

void Shader::setVertexAttribDivisor(const std::string &name, const GLuint divisor) {
    if(this->mapping.find(name) == this->mapping.end()) {
        const GLint location = glGetAttribLocation(this->shaderID, name.c_str());
        if(location<0)
            return;

        this->mapping[name] = location;
    }

    setVertexAttribDivisor(static_cast<const GLuint>(this->mapping[name]), divisor);
}

void Shader::setVertexAttribDivisor(const std::string &name, const GLuint count, const GLuint divisor) {
    if(this->mapping.find(name) == this->mapping.end()) {
        const GLint location = glGetAttribLocation(this->shaderID, name.c_str());
        if(location<0)
            return;

        this->mapping[name] = location;
    }

    setVertexAttribDivisor(static_cast<const GLuint>(this->mapping[name]), count, divisor);
}

GLuint Shader::compileComputeShader(const std::string &computeCode) {
    int success;
    char infoLog[GL_LOG_SIZE];

    const char *vComputeCode = computeCode.c_str();

    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &vComputeCode, nullptr);
    glCompileShader(computeShader);
    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(computeShader, GL_LOG_SIZE, nullptr, infoLog);
        logger->error("Compute Shader compilation failed:\n{}", infoLog);
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, computeShader);
    glLinkProgram(program);
    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderID, GL_LOG_SIZE, nullptr, infoLog);
        logger->error("Shader linking failed:\n{}", infoLog);
    }

    glDeleteShader(computeShader);
    return program;
}
