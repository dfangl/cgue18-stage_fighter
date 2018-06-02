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
    std::shared_ptr<Shader> shader;

    GLuint VAO;
    GLuint SSBO;

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

    // Default == pure virtual?
    virtual void generateParticles(unsigned int count);

public:
    ParticleSystem(const glm::vec3 &position, float radius, std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture,
                   unsigned int particles);
    virtual ~ParticleSystem();

    void render(Scene *scene) override;

    void draw() override;
};


#endif //STAGE_FIGHTER_PARTICLESYSTEM_H
