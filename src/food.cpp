#include "food.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

namespace naturalselection {
using glm::vec2;

Food::Food(vec2 new_position, float new_radius, ci::Color new_color) {
    position_ = new_position;
    radius_ = new_radius;
    color_ = new_color;

}

std::vector<Food> Food::SpawnParticles(size_t count, ci::Color color, float radius, int edge_buffer) {
    srand((unsigned int) time(NULL));
    std::vector<Food> food_particles;
    for (size_t i = 0; i < count; i++) {
        // Creates two random x and y coordinates within the gas container box.
        int random_x = rand() % (DEFAULT_WIDTH - (2 * edge_buffer)) + (DEFAULT_X_COOR + edge_buffer);
        int random_y = rand() % (DEFAULT_HEIGHT - (2 * edge_buffer)) + (DEFAULT_Y_COOR + edge_buffer);

        // Set position and velocity vectors according to random generators.
        vec2 position = vec2(random_x, random_y);

        Food new_particle = Food(position, radius, color);

        // Add particle to vector.
        food_particles.push_back(new_particle);
    }

    return food_particles;
}

glm::vec2 Food::GetPosition() const {
    return position_;
}

float Food::GetRadius() const {
    return radius_;
}

ci::Color Food::GetColor() const {
    return color_;
}

}