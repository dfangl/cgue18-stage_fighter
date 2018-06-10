//
// Created by Raphael on 02.06.2018.
//

#ifndef STAGE_FIGHTER_PARTICLESYSTEM_H
#define STAGE_FIGHTER_PARTICLESYSTEM_H

#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "../Texture.h"

#include "Object3D.h"

class ParticleSystem : public Object3D {

private:
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Shader> compute;
    //std::shared_ptr<Shader> shader;

    GLuint VAO  = 0;
    GLuint SSBO = 0;

protected:
    struct Particle {
        glm::vec4 position;
        glm::vec4 velocity;
        glm::vec4 startPosition;

        Particle() = default;
        Particle(const glm::vec4 &p, const glm::vec4 &v, const glm::vec4 &s)
                : position(p), velocity(v), startPosition(s) {}
    };

    unsigned int particles;
    std::vector<Particle> data;

    glm::vec2 particle_size = glm::vec2(7.0f, 7.0f);

    // Default == pure virtual?
    virtual void generateParticles(unsigned int count);

    void loadSSBO();

public:
    ParticleSystem(const glm::vec3 &position, float radius, std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture,
                   unsigned int particles);
    virtual ~ParticleSystem();

    void render(Scene *scene) override;

    void draw() override;

    void setSize(const glm::vec2 &);
};


#endif //STAGE_FIGHTER_PARTICLESYSTEM_H
