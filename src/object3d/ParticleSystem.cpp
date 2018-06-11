//
// Created by Raphael on 02.06.2018.
//

#include "ParticleSystem.h"
#include <stb_perlin.h>
#include <utility>
#include <random>

#include "../Scene.h"
#include "../manager/ShaderManager.h"

#include "../helper/CompilerMacros.h"

ParticleSystem::ParticleSystem(const glm::vec3 &position, float radius, std::shared_ptr<Shader> shader,
                               std::shared_ptr<Texture> texture, unsigned int count) :
    Object3D(position, radius, shader) {

    // TODO: move to constructor or let child initialize
    this->compute = ShaderManager::loadCompute("particlesystem-compute");
    this->particles = count;
    this->texture = std::move(texture);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &SSBO);
    //generateParticles(count);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, SSBO);

    // Position
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
    glEnableVertexAttribArray(0);

    // Max TTL
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void *)(11 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    shader->use();
    shader->setUniform("texture_0", 0);
    glUseProgram(0);
}

ParticleSystem::~ParticleSystem() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &SSBO);
}

void ParticleSystem::generateParticles(unsigned int count) {
    data.reserve(count);

    float nz1 = 29.2938648971363f;
    float nz2 = 24.1293682342339f;
    float nz3 = 39.3745902239486f;

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(2000, 3499);

    for(int i=0; i < count; i++) {
        float nx = (float)i / count * 1.128128364f;
        float ny = (float)i / count * 0.923865875f;

        float speed = stb_perlin_ridge_noise3(18 * nx, 18 * ny, 5 * nz1, 2.0, 0.5, 1.0, 24, 18, 18, 64) * 400;
        float ttl   = distribution(generator);
        float noiseX = ( 0.5f - stb_perlin_ridge_noise3(9 * nx, 9 * ny, nz1, 2.0, 0.52734, 1.0, 24, 9, 9, 64) ) / speed;
        float noiseY = ( 0.5f - stb_perlin_ridge_noise3(9 * nx, 9 * ny, nz2, 1.9, 0.49237, 1.0, 24, 9, 9, 64) ) / speed;
        float noiseZ = ( 0.5f - stb_perlin_ridge_noise3(8 * nx, 9 * ny, nz3, 1.8, 0.51278, 1.0, 24, 9, 9, 64) ) / speed;

        float x = static_cast<float>(glm::cos(distribution(generator))) * 1.4f;
        float y = static_cast<float>(glm::sin(distribution(generator))) * 1.4f;
        float z = static_cast<float>(glm::tan(distribution(generator))) * 1.4f;

        data.emplace_back(
                glm::vec4(x, y, z, ttl - distribution(generator)),
                glm::vec4(noiseX, noiseY, noiseZ, 0.0f),
                glm::vec4(x, y, z, ttl)

        );
    }

    loadSSBO();
}

void ParticleSystem::update(Scene *scene) {
    auto hpDeltaT = static_cast<float>(scene->getFrameTime().count());

    // Compute Particle stuff:
    compute->use();
    compute->setUniform("deltaT", hpDeltaT);
    compute->setUniform("size", particles);
    compute->setUniform("enableRespawn", (unsigned int) true);
    const int workingGroups = particles;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
    glDispatchCompute((GLuint) workingGroups, 1, 1);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

    // Memory Barrier
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
}

void ParticleSystem::draw() {
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    texture->bind(GL_TEXTURE0);
    shader->setUniform("particle_size", this->particle_size);

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, particles);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

void ParticleSystem::setSize(const glm::vec2 &size) {
    this->particle_size = size;
}

void ParticleSystem::loadSSBO() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(ParticleSystem::Particle), data.data(), GL_STATIC_DRAW);
}

