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


class AbstractParticleSystem : virtual public Object3DAbstract {

protected:
    virtual void generateParticles(unsigned int count)  = 0;

public:
    virtual void update(Scene *scene) = 0;

};



class ParticleSystem : virtual public AbstractParticleSystem, public Object3D {

private:
    std::shared_ptr<Shader> compute;
    //std::shared_ptr<Shader> shader;

    //GLuint VAO  = 0;
    GLuint SSBO = 0;

protected:
    std::shared_ptr<Texture> texture;

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
    ParticleSystem(const glm::vec3 &position, float radius, std::vector<std::shared_ptr<Shader>> shader, std::shared_ptr<Texture> texture,
                   unsigned int particles);
    virtual ~ParticleSystem();

    void update(Scene *scene);
    void draw(std::shared_ptr<Shader> &shader) override;
    void setSize(const glm::vec2 &);

    void uploadSSBOtoGPU() { this->particles = static_cast<unsigned int>(data.size()); this->loadSSBO(); }
};


#endif //STAGE_FIGHTER_PARTICLESYSTEM_H
