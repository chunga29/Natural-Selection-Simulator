#include "physics.h"
#include "creature.h"

namespace naturalselection {

using glm::vec2;

// Source: Stackoverflow
float Physics::CalculateDistance(float x1, float y1, float x2, float y2) {
    // Calculating distance
    return (float) sqrt(pow(x2 - x1, 2) +
                pow(y2 - y1, 2) * 1.0);
}

// Source: Stackoverflow
double Physics::fRand(double fMin, double fMax) {
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

vec2 Physics::GetFlippedXVelocity(Creature particle) {
    // Negates X Value
    float new_x_vel = particle.GetVelocity().x * (-1);
    float new_y_vel = particle.GetVelocity().y;

    vec2 new_velocity = vec2(new_x_vel, new_y_vel);
    return new_velocity;
}

vec2 Physics::GetFlippedYVelocity(Creature particle) {
    // Negates Y Value
    float new_x_vel = particle.GetVelocity().x;
    float new_y_vel = particle.GetVelocity().y * (-1);

    vec2 new_velocity = vec2(new_x_vel, new_y_vel);
    return new_velocity;
}

std::vector<vec2> Physics::CalculateParticleBounceVelocity(Creature particle_1, Creature particle_2) {
    std::vector<vec2> new_velocities;
    vec2 position_1 = particle_1.GetPosition();
    vec2 position_2 = particle_2.GetPosition();

    vec2 velocity_1 = particle_1.GetVelocity();
    vec2 velocity_2 = particle_2.GetVelocity();

    float mass_1 = particle_1.GetMass();
    float mass_2 = particle_2.GetMass();

    // Calculates the new velocity of the first particle.
    vec2 new_velocity_1 = velocity_1 - ((2 * mass_2) / (mass_1 + mass_2)) * (glm::dot((velocity_1 - velocity_2),
                                      (position_1 - position_2))/
                                      (float) pow(glm::length(position_1 - position_2), 2))
                                                * (position_1 - position_2);

    // Calculates the new velocity of the second particle.
    vec2 new_velocity_2 = velocity_2 - ((2 * mass_1) / (mass_1 + mass_2)) * (glm::dot((velocity_2 - velocity_1),
                                      (position_2 - position_1))/
                                      (float) pow(glm::length(position_2 - position_1), 2))
                                                * (position_2 - position_1);

    // Add to new velocity vector.
    new_velocities.push_back(new_velocity_1);
    new_velocities.push_back(new_velocity_2);
    return new_velocities;
}

float Physics::FindHighestSpeed(std::vector<Creature> particles) {
    if (!particles.empty()) {
        float highest_velocity = glm::length(particles.at(0).GetVelocity());
        for (size_t i = 1; i < particles.size(); i++) {
            float new_velocity = glm::length(particles.at(i).GetVelocity());
            if (new_velocity > highest_velocity) { // If a new, higher velocity is found,
                highest_velocity = new_velocity; // replace the temp value.
            }
        }
        return highest_velocity;
    } else {
        return 0.0;
    }
}

float Physics::FindLowestSpeed(std::vector<Creature> particles) {
    if (!particles.empty()) {
        float lowest_velocity = glm::length(particles.at(0).GetVelocity());
        for (size_t i = 1; i < particles.size(); i++) {
            float new_velocity = glm::length(particles.at(i).GetVelocity());
            if (new_velocity < lowest_velocity) { // If a new, lower velocity is found,
                lowest_velocity = new_velocity; // replace the temp value.
            }
        }
        return lowest_velocity;
    } else {
        return 0.0;
    }
}

}