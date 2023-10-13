#include "creature.h"


namespace naturalselection {

using glm::vec2;

Creature::Creature() {

}

Creature::Creature(int creature_type, vec2 new_position, vec2 new_velocity, float new_radius,
                   float new_mass, ci::Color new_color, double current_energy, int current_food) {
    creature_type_ = creature_type;
    position_ = new_position;
    velocity_ = new_velocity;
    radius_ = new_radius;
    mass_ = new_mass;
    color_ = new_color;
    current_energy_ = current_energy;
    current_food_ = current_food;
    max_velocity_ = DEFAULT_MAX_VELOCITY;
    energy_spend_ = DEFAULT_ENERGY_SPEND;
    vision_radius_ = DEFAULT_VISION_RADIUS;
    needs_movement_ = true;
}

Creature::Creature(int creature_type, vec2 new_position, vec2 new_velocity, float new_radius,
                   float new_mass, ci::Color new_color, double current_energy, int current_food, float max_velocity, double energy_spend) {
    creature_type_ = creature_type;
    position_ = new_position;
    velocity_ = new_velocity;
    radius_ = new_radius;
    mass_ = new_mass;
    color_ = new_color;
    current_energy_ = current_energy;
    current_food_ = current_food;
    max_velocity_ = max_velocity;
    energy_spend_ = energy_spend;
    vision_radius_ = DEFAULT_VISION_RADIUS;
    needs_movement_ = true;
}

Creature::Creature(int creature_type, vec2 new_position, vec2 new_velocity, float new_radius,
                   float new_mass, ci::Color new_color, double current_energy, int current_food, double vision_radius,
                   double energy_spend) {
    creature_type_ = creature_type;
    position_ = new_position;
    velocity_ = new_velocity;
    radius_ = new_radius;
    mass_ = new_mass;
    color_ = new_color;
    current_energy_ = current_energy;
    current_food_ = current_food;
    max_velocity_ = DEFAULT_MAX_VELOCITY;
    energy_spend_ = energy_spend;
    needs_movement_ = true;
    vision_radius_ = vision_radius;
}

Creature::Creature(int creature_type, vec2 new_position, vec2 new_velocity, float new_radius,
                   float new_mass, ci::Color new_color, double current_energy, int current_food, double vision_radius,
                   double energy_spend, float max_velocity) {
    creature_type_ = creature_type;
    position_ = new_position;
    velocity_ = new_velocity;
    radius_ = new_radius;
    mass_ = new_mass;
    color_ = new_color;
    current_energy_ = current_energy;
    current_food_ = current_food;
    max_velocity_ = DEFAULT_MAX_VELOCITY;
    energy_spend_ = energy_spend;
    needs_movement_ = true;
    vision_radius_ = vision_radius;
    max_velocity_ = max_velocity;
}

std::vector<Creature> Creature::SpawnCreatures(int creature_type, size_t count, ci::Color color, float radius, float mass,
                                               double current_energy, int current_food) {
    std::vector<Creature> particles;
    for (size_t i = 0; i < count; i++) {
        int random_side = rand() % 4; // 0 - 3 (N,S,E,W)

        int x_coor;
        int y_coor;

        double x_vel;
        double y_vel;

        if (random_side == 0) { // North
            x_coor = rand() % DEFAULT_WIDTH + DEFAULT_X_COOR;
            y_coor = DEFAULT_Y_COOR;
            x_vel = 0;
            y_vel = DEFAULT_MAX_VELOCITY;
        } else if (random_side == 1) { // South
            x_coor = rand() % DEFAULT_WIDTH + DEFAULT_X_COOR;
            y_coor = DEFAULT_HEIGHT + DEFAULT_Y_COOR;
            x_vel = 0;
            y_vel = -DEFAULT_MAX_VELOCITY;
        } else if (random_side == 2) { // East
            x_coor = DEFAULT_X_COOR;
            y_coor = rand() % DEFAULT_HEIGHT + DEFAULT_Y_COOR;
            x_vel = DEFAULT_MAX_VELOCITY;
            y_vel = 0;
        } else { // West
            x_coor = DEFAULT_WIDTH + DEFAULT_X_COOR;
            y_coor = rand() % DEFAULT_HEIGHT + DEFAULT_Y_COOR;
            x_vel = -DEFAULT_MAX_VELOCITY;
            y_vel = 0;
        }

        // Set position and velocity vectors according to random generators.
        vec2 position = vec2(x_coor, y_coor);
        vec2 velocity = vec2(x_vel, y_vel);

        Creature new_particle = Creature(creature_type, position, velocity, radius,
                                         mass, color, current_energy, current_food);

        // Add particle to vector.
        particles.push_back(new_particle);
    }

    return particles;
}

void Creature::ChangeVelocityTowardsNearestWall() {
    float x_pos = position_.x;
    float y_pos = position_.y;

    float bottom_distance = DEFAULT_HEIGHT - (y_pos - DEFAULT_Y_COOR);
    float top_distance = DEFAULT_HEIGHT - bottom_distance;

    float right_distance = DEFAULT_WIDTH - (x_pos - DEFAULT_X_COOR);
    float left_distance = DEFAULT_WIDTH - right_distance;

    float closest = std::min(left_distance, std::min(right_distance, std::min(bottom_distance, top_distance)));

    if (closest == bottom_distance) {
        velocity_ = vec2(0, max_velocity_);
    } else if (closest == top_distance) {
        velocity_ = vec2(0, -max_velocity_);
    } else if (closest == right_distance) {
        velocity_ = vec2(max_velocity_, 0);
    } else {
        velocity_ = vec2(-max_velocity_, 0);
    }
}

void Creature::ChangeVelocityTowardsFurthestCorner() {
    float x_pos = position_.x;
    float y_pos = position_.y;

    float top_left = CalculateDistance(x_pos, y_pos, DEFAULT_X_COOR, DEFAULT_Y_COOR);
    float top_right = CalculateDistance(x_pos, y_pos, DEFAULT_X_COOR + DEFAULT_WIDTH, DEFAULT_Y_COOR);

    float bottom_left = CalculateDistance(x_pos, y_pos, DEFAULT_X_COOR, DEFAULT_Y_COOR + DEFAULT_HEIGHT);
    float bottom_right = CalculateDistance(x_pos, y_pos, DEFAULT_X_COOR + DEFAULT_WIDTH, DEFAULT_Y_COOR + DEFAULT_HEIGHT);

    float closest = std::max(top_left, std::max(top_right, std::max(bottom_left, bottom_right)));

    if (closest == top_left) {
        ChangeVelocityTowardCoordinates(vec2(DEFAULT_X_COOR, DEFAULT_Y_COOR));
    } else if (closest == top_right) {
        ChangeVelocityTowardCoordinates(vec2(DEFAULT_X_COOR + DEFAULT_WIDTH, DEFAULT_Y_COOR));
    } else if (closest == bottom_left) {
        ChangeVelocityTowardCoordinates(vec2(DEFAULT_X_COOR, DEFAULT_Y_COOR + DEFAULT_HEIGHT));
    } else {
        ChangeVelocityTowardCoordinates(vec2(DEFAULT_X_COOR + DEFAULT_WIDTH, DEFAULT_Y_COOR + DEFAULT_HEIGHT));
    }
}

void Creature::ChangeVelocityTowardCoordinates(vec2 coors) {
    vec2 length = coors - position_;
    vec2 new_velocity = glm::normalize(length) * max_velocity_;

    velocity_ = new_velocity;
}

bool Creature::ChangeVelocityTowardsNearestFood(std::vector<Food> food) {
    Food nearest_food = FindNearestFood(food);

    if (CalculateDistance(position_.x, position_.y, // IF NEAREST FOOD IS WITHIN 25 UNITS
                          nearest_food.GetPosition().x, nearest_food.GetPosition().y) <= vision_radius_) {
        vec2 length = nearest_food.GetPosition() - position_;
        vec2 new_velocity = glm::normalize(length) * max_velocity_;

        velocity_ = new_velocity;
        return true;
    }

    return false;
}

Food Creature::FindNearestFood(std::vector<Food> food) { // Make sure no food vector is empty for this.
    Food nearest_food = food.at(0);
    float nearest_distance = std::numeric_limits<float>::infinity();

    for (size_t i = 0; i < food.size(); i++) {
        float x1 = food.at(i).GetPosition().x;
        float y1 = food.at(i).GetPosition().y;
        float x2 = position_.x;
        float y2= position_.y;

        float new_distance = CalculateDistance(x1, y1, x2, y2);
        if (nearest_distance > new_distance) {
            nearest_distance = new_distance;
            nearest_food = food.at(i);
        }
    }

    return nearest_food;
}

bool Creature::IsOnFood(std::vector<Food> food) {
    float x_pos_1 = position_.x;
    float y_pos_1 = position_.y;
    float radius_1 = radius_;

    for (size_t i = 0; i < food.size(); i++) {
        float x_pos_2 = food.at(i).GetPosition().x;
        float y_pos_2 = food.at(i).GetPosition().y;
        float radius_2 = food.at(i).GetRadius();
        if (CalculateDistance(x_pos_1, y_pos_1, x_pos_2, y_pos_2) <= radius_1 + radius_2) {
            return true;
        }
    }

    return false;
}

bool Creature::IsTouchingSpecificFood(Food food) {
    float x_pos_1 = position_.x;
    float y_pos_1 = position_.y;
    float radius_1 = radius_;

    float x_pos_2 = food.GetPosition().x;
    float y_pos_2 = food.GetPosition().y;
    float radius_2 = food.GetRadius();
    if (CalculateDistance(x_pos_1, y_pos_1, x_pos_2, y_pos_2) <= radius_1 + radius_2) {
        return true;
    }

    return false;
}

void Creature::ChangeVelocityIfNotEnoughEnergy() {
    float x_pos = position_.x;
    float y_pos = position_.y;

    float bottom_distance = DEFAULT_HEIGHT - (y_pos - DEFAULT_Y_COOR);
    float top_distance = DEFAULT_HEIGHT - bottom_distance;

    float right_distance = DEFAULT_WIDTH - (x_pos - DEFAULT_X_COOR);
    float left_distance = DEFAULT_WIDTH - right_distance;

    float closest = std::min(left_distance, std::min(right_distance, std::min(bottom_distance, top_distance)));

    double energy_needed = (closest / max_velocity_) * energy_spend_;

    if (closest == bottom_distance) {
        if (energy_needed >= current_energy_) {
            velocity_ = vec2(0, max_velocity_);
        }
    } else if (closest == top_distance) {
        if (energy_needed >= current_energy_) {
            velocity_ = vec2(0, -max_velocity_);
        }
    } else if (closest == right_distance) {
        if (energy_needed >= current_energy_) {
            velocity_ = vec2(max_velocity_, 0);
        }
    } else {
        if (energy_needed >= current_energy_) {
            velocity_ = vec2(-max_velocity_, 0);
        }
    }
}

bool Creature::ChangeVelocityIfEnoughFood() {
    if (current_food_ == 2) {
        ChangeVelocityTowardsNearestWall();
        return true;
    }

    return false;
}

// Source: Stackoverflow
float Creature::CalculateDistance(float x1, float y1, float x2, float y2) {
    // Calculating distance
    return (float) sqrt(pow(x2 - x1, 2) +
                        pow(y2 - y1, 2) * 1.0);
}


glm::vec2 Creature::GetPosition() const {
    return position_;
}

glm::vec2 Creature::GetVelocity() const {
    return velocity_;
}

float Creature::GetMass() const {
    return mass_;
}

float Creature::GetRadius() const {
    return radius_;
}

ci::Color Creature::GetColor() const {
    return color_;
}

double Creature::GetEnergy() const {
    return current_energy_;
}

int Creature::GetFood() const {
    return current_food_;
}

bool Creature::GetNeedsMovement() const {
    return needs_movement_;
}

float Creature::GetMaxVelocity() const {
    return max_velocity_;
}

double Creature::GetVisionRadius() const {
    return vision_radius_;
}

int Creature::GetCreatureType() const {
    return creature_type_;
}

double Creature::GetEnergySpend() const {
    return energy_spend_;
}

void Creature::AddFood() {
    current_food_++;
    if (creature_type_ == SPEED) {
        if (current_food_ == 1) {
            color_ = ci::Color(1.0f, 0.4f, 0.4f);
        } else if (current_food_ == 2) {
            color_ = ci::Color(1.0f, 0.6f, 0.6f);
        }
    } else if (creature_type_ == INTELLIGENCE) {
        if (current_food_ == 1) {
            color_ = ci::Color(0.4f, 0.4f, 1.0f);
        } else if (current_food_ == 2) {
            color_ = ci::Color(0.6f, 0.6f, 1.0f);
        }
    } else if (creature_type_ == BOTH) {
        if (current_food_ == 1) {
            color_ = ci::Color(0.8f, 0.4f, 0.8f);
        } else if (current_food_ == 2) {
            color_ = ci::Color(0.8f, 0.6f, 0.8f);
        }
    }
}

void Creature::SetPosition(glm::vec2 new_position) {
    position_ = new_position;
}

void Creature::SetVelocity(glm::vec2 new_velocity) {
    velocity_ = new_velocity;
}

void Creature::SetMass(float new_mass) {
    mass_ = new_mass;
}

void Creature::SetRadius(float new_radius) {
    radius_ = new_radius;
}

void Creature::SetColor(ci::Color new_color) {
    color_ = new_color;
}

void Creature::SetEnergy(double new_energy) {
    current_energy_ = new_energy;
}

void Creature::SetFood(int new_food) {
    current_food_ = new_food;
}

void Creature::SetNeedsMovement(bool new_movement) {
    needs_movement_ = new_movement;
}

void Creature::SetVisionRadius(double new_radius) {
    vision_radius_ = new_radius;
}

void Creature::Move() {
    if (current_energy_ > 0) {
        current_energy_ -= energy_spend_; // 1000 / 2.5
    }
    position_ += velocity_;
}

void Creature::ResetCreaturePosition() {
    int random_side = rand() % 4; // 0 - 3 (N,S,E,W)

    int x_coor;
    int y_coor;

    double x_vel;
    double y_vel;

    if (random_side == 0) { // North
        x_coor = rand() % DEFAULT_WIDTH + DEFAULT_X_COOR;
        y_coor = DEFAULT_Y_COOR;
        x_vel = 0;
        y_vel = max_velocity_;
    } else if (random_side == 1) { // South
        x_coor = rand() % DEFAULT_WIDTH + DEFAULT_X_COOR;
        y_coor = DEFAULT_HEIGHT + DEFAULT_Y_COOR;
        x_vel = 0;
        y_vel = -max_velocity_;
    } else if (random_side == 2) { // East
        x_coor = DEFAULT_X_COOR;
        y_coor = rand() % DEFAULT_HEIGHT + DEFAULT_Y_COOR;
        x_vel = max_velocity_;
        y_vel = 0;
    } else { // West
        x_coor = DEFAULT_WIDTH + DEFAULT_X_COOR;
        y_coor = rand() % DEFAULT_HEIGHT + DEFAULT_Y_COOR;
        x_vel = -max_velocity_;
        y_vel = 0;
    }

    // Set position and velocity vectors according to random generators.
    vec2 position = vec2(x_coor, y_coor);
    vec2 velocity = vec2(x_vel, y_vel);

    position_ = position;
    velocity_ = velocity;
}

void Creature::ResetAllCreatures(std::vector<Creature> &creatures) {
    for (size_t i = 0; i < creatures.size(); i++) {
        creatures.at(i).ResetCreaturePosition();
        if (creatures.at(i).GetCreatureType() == SPEED) {
            creatures.at(i).SetColor(ci::Color("Red"));
        } else if (creatures.at(i).GetCreatureType() == INTELLIGENCE) {
            creatures.at(i).SetColor(ci::Color("Blue"));
        } else if (creatures.at(i).GetCreatureType() == BOTH) {
            creatures.at(i).SetColor(ci::Color(ci::Color(0.8f, 0.0f, 0.8f)));
        }
        creatures.at(i).SetEnergy(DEFAULT_ENERGY_CAPACITY);
        creatures.at(i).SetFood(0);
    }
}

Creature Creature::CreateChild(int creature_type) {
    if (creature_type == SPEED) { // SPEED CHILD
        float max_velocity = GetMaxVelocity();
        float margins = max_velocity / 10; // 10% faster or slower
        float new_max_velocity = RandomFloatRange(max_velocity - margins, max_velocity + margins); // Random mutation
        double new_energy_spend = DEFAULT_ENERGY_SPEND;

        float difference = new_max_velocity - DEFAULT_MAX_VELOCITY; // Difference from default velocity

        if (difference > 0) { // Increase in speed: Exponentially increased cost
            double multiplier = (1 + (difference / DEFAULT_MAX_VELOCITY)); // Percentage change
            new_energy_spend = DEFAULT_ENERGY_SPEND * pow(multiplier, 2);
        } else if (difference < 0) { // Decrease in speed: Logarithmically decreasing cost
            float log_value = 1 + (-difference / DEFAULT_MAX_VELOCITY);
            float energy_decrease = (float) DEFAULT_ENERGY_SPEND * log(log_value);
            new_energy_spend = DEFAULT_ENERGY_SPEND - energy_decrease;
        }

        return Creature(SPEED, vec2(0, 0), vec2(0, 0), GetRadius(),
                             GetMass(), GetColor(), 0.0, 0,
                        new_max_velocity, new_energy_spend);
    } else if (creature_type == INTELLIGENCE) { // INTELLIGENCE CHILD
        double vision_radius = GetVisionRadius();
        double margins = vision_radius / 2; // 50% faster or slower
        double new_vision_radius = (double) RandomFloatRange((float) (vision_radius - margins), (float)(vision_radius + margins)); // Random mutation
        double new_energy_spend = DEFAULT_ENERGY_SPEND;

        double difference = vision_radius - DEFAULT_VISION_RADIUS; // Difference from default velocity

        if (difference > 0) { // Increase in vision: Logarithmically increased cost
            float log_value = 1 + (float) (difference / DEFAULT_VISION_RADIUS);
            float energy_increase = (float) DEFAULT_ENERGY_SPEND * log(log_value);
            new_energy_spend = DEFAULT_ENERGY_SPEND + energy_increase;
        } else if (difference < 0) { // Decrease in vision: Logarithmically decreasing cost
            float log_value = 1 + (float) (-difference / DEFAULT_VISION_RADIUS);
            float energy_decrease = (float) DEFAULT_ENERGY_SPEND * log(log_value);
            new_energy_spend = DEFAULT_ENERGY_SPEND - energy_decrease;
        }

        return Creature(INTELLIGENCE, vec2(0, 0), vec2(0, 0), GetRadius(),
                        GetMass(), GetColor(), 0.0, 0,
                        new_vision_radius, new_energy_spend);
    } else if (creature_type == BOTH) {
        float max_velocity = GetMaxVelocity();
        float margins = max_velocity / 10; // 10% faster or slower
        float new_max_velocity = RandomFloatRange(max_velocity - margins, max_velocity + margins); // Random mutation
        double new_energy_spend = DEFAULT_ENERGY_SPEND;

        float difference = new_max_velocity - DEFAULT_MAX_VELOCITY; // Difference from default velocity

        if (difference > 0) { // Increase in speed: Exponentially increased cost
            double multiplier = (1 + (difference / DEFAULT_MAX_VELOCITY)); // Percentage change
            new_energy_spend = DEFAULT_ENERGY_SPEND * pow(multiplier, 2);
        } else if (difference < 0) { // Decrease in speed: Logarithmically decreasing cost
            float log_value = 1 + (-difference / DEFAULT_MAX_VELOCITY);
            float energy_decrease = (float) DEFAULT_ENERGY_SPEND * log(log_value);
            new_energy_spend = DEFAULT_ENERGY_SPEND - energy_decrease;
        }

        Creature middle = Creature(SPEED, vec2(0, 0), vec2(0, 0), GetRadius(),
                        GetMass(), GetColor(), 0.0, 0,
                        new_max_velocity, new_energy_spend);

        double vision_radius = middle.GetVisionRadius();
        double margins_middle = vision_radius / 2; // 50% faster or slower
        double new_vision_radius_middle = (double) RandomFloatRange((float) (vision_radius - margins_middle), (float)(vision_radius + margins_middle)); // Random mutation
        double new_energy_spend_middle = DEFAULT_ENERGY_SPEND;

        double difference_middle = vision_radius - DEFAULT_VISION_RADIUS; // Difference from default velocity

        if (difference_middle > 0) { // Increase in vision: Logarithmically increased cost
            float log_value = 1 + (float) (difference_middle / DEFAULT_VISION_RADIUS);
            float energy_increase = (float) DEFAULT_ENERGY_SPEND * log(log_value);
            new_energy_spend_middle = DEFAULT_ENERGY_SPEND + energy_increase;
        } else if (difference_middle < 0) { // Decrease in vision: Logarithmically decreasing cost
            float log_value = 1 + (float) (-difference_middle / DEFAULT_VISION_RADIUS);
            float energy_decrease = (float) DEFAULT_ENERGY_SPEND * log(log_value);
            new_energy_spend_middle = DEFAULT_ENERGY_SPEND - energy_decrease;
        }

        return Creature(BOTH, vec2(0, 0), vec2(0, 0), GetRadius(),
                        GetMass(), GetColor(), 0.0, 0,
                        new_vision_radius_middle, new_energy_spend_middle, new_max_velocity);
    } else {
        return Creature();
    }
}

float Creature::RandomFloatRange(float a, float b) {
    return ((b - a) * ((float)rand() / RAND_MAX)) + a;
}


}