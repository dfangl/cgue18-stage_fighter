//
// Created by Raphael on 02.06.2018.
//

#include "ParticleSystem.h"
#include <stb_perlin.h>
#include <utility>
#include "../Scene.h"
#include "../manager/ShaderManager.h"

void ParticleSystem::render(Scene *scene) {
    auto hpDeltaT = static_cast<float>(scene->getFrameTime().count());

    // Compute Particle stuff:
    compute->use();
    compute->setUniform("deltaT", hpDeltaT);
    compute->setUniform("size", particles);
    const int workingGroups = particles;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
    glDispatchCompute((GLuint) workingGroups, 1, 1);
    glUseProgram(0);

    // Memory Barrier
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    // Display Particles:
    Object3D::render(scene);
}

ParticleSystem::ParticleSystem(const glm::vec3 &position, float radius, std::shared_ptr<Shader> shader,
                               std::shared_ptr<Texture> texture, unsigned int count) :
    Object3D(position, radius, shader) {

    // TODO: move to constructor or let child initialize
    this->compute = ShaderManager::loadCompute("particlesystem-compute");
    this->particles = count;
    this->texture = std::move(texture);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &SSBO);
    generateParticles(count);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, SSBO);

    // Position
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
    glEnableVertexAttribArray(0);

    // Max TTL
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)(11 * sizeof(float)));
    glEnableVertexAttribArray(1);

    shader->setUniform("texture_0", 0);

    glBindVertexArray(0);
}

ParticleSystem::~ParticleSystem() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &SSBO);
}

void ParticleSystem::generateParticles(unsigned int count) {
    data.reserve(count);

    for(int i=0; i < count; i++) {
        float nx = (float)i / count * 1.128128364f;
        float ny = (float)i / count * 0.923865875f;
        float nz = 29.2938648971363f;

        float speed = stb_perlin_ridge_noise3(18 * nx, 18 * ny, 5 * nz, 2.0, 0.5, 1.0, 24, 18, 18, 64) * 300;
        float noiseX = ( 0.5f - stb_perlin_ridge_noise3(2 * nx, 2 * ny, nz, 2.0, 0.5, 1.0, 24, 2, 2, 64) ) / speed;
        float noiseY = ( 0.5f - stb_perlin_ridge_noise3(2 * nx, 9 * ny, nz, 2.0, 0.5, 1.0, 24, 4, 4, 64) ) / speed;
        float noiseZ = ( 0.5f - stb_perlin_ridge_noise3(9 * nx, 2 * ny, nz, 2.0, 0.5, 1.0, 24, 4, 4, 64) ) / speed;

        data.emplace_back(
                glm::vec4(this->position, 2000.0f),
                glm::vec4(noiseX, noiseY, noiseZ, 0.0f),
                glm::vec4(this->position, 2000.0f)

        );
    }

    spdlog::get("console")->info("DATA: {}, {}", (void *)data.data(), data.size());

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, count * sizeof(ParticleSystem::Particle), data.data(), GL_STATIC_DRAW);
}

void ParticleSystem::draw() {
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    texture->bind(GL_TEXTURE0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, particles);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}


