//
// Created by Raphael on 25.03.2018.
//

#include "GLDebugDrawer.h"

#include <glad/glad.h>

GLDebugDrawer::GLDebugDrawer() : Logger("Bullet"), Object3D(glm::vec3(0,0,0), 0.0f, ShaderManager::load("bulletdebug")) {
    this->debugMode = btIDebugDraw::DBG_NoDebug;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &colorVBO);
}

void GLDebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {
    this->lines.emplace_back(glm::vec3(from.x(), from.y(), from.z()), glm::vec3(to.x(), to.y(), to.z()));
    this->colors.emplace_back(color.x(), color.y(), color.z());
}

void GLDebugDrawer::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime,
                                const btVector3 &color) {
    // TODO: Implement me
}

void GLDebugDrawer::reportErrorWarning(const char *warningString) {
    logger->error("{}", warningString);
}

void GLDebugDrawer::draw3dText(const btVector3 &location, const char *textString) {
    // TODO: Implement me
}

void GLDebugDrawer::setDebugMode(int debugMode) {
    logger->info("Setting Debug Mode to {}", debugMode);
    this->debugMode = debugMode;
}

int GLDebugDrawer::getDebugMode() const {
    return this->debugMode;
}

GLDebugDrawer::~GLDebugDrawer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void GLDebugDrawer::draw() {

    glDisable(GL_CULL_FACE);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Line) * lines.size(), lines.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3[2]), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colors.size(), colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lines.size()));

    glBindVertexArray(0);

    lines.clear();
    colors.clear();
}
