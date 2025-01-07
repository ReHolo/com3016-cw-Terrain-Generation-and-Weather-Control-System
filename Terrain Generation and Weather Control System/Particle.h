#pragma once
#include <glm/glm.hpp>

struct Particle
{
    glm::vec3 position;
    glm::vec3 velocity;

    Particle()
        : position(0.0f), velocity(0.0f) {}
    Particle(const glm::vec3& pos, const glm::vec3& vel)
        : position(pos), velocity(vel) {}

    
};
