#include "environment.h"
#include "creature.h"
#include "physics.h"
#include "speed_histogram.h"

namespace naturalselection {

using glm::vec2;

Environment::Environment() {
    width_ = DEFAULT_WIDTH;
    height_ = DEFAULT_HEIGHT;
    x_coor_ = DEFAULT_X_COOR;
    y_coor_ = DEFAULT_Y_COOR;
    is_running_ = false;
    needs_reset = false;
    food_count_ = DEFAULT_FOOD_COUNT;

    food_ = Food::SpawnParticles(DEFAULT_FOOD_COUNT, ci::Color("Green"), 2.0f, 20);

    // Population Graph
    std::vector<std::vector<Creature>> population_records;
    population_records.push_back(creatures_);
    population_records_.insert(population_records_.end(), population_records.begin(), population_records.end());
    population_graphs_.push_back(PopulationGraph("Trials", DEFAULT_HISTOGRAM_WIDTH * 2,
                                                 DEFAULT_HISTOGRAM_HEIGHT * 2, 1000,
                                                 DEFAULT_Y_COOR * 2 + DEFAULT_HISTOGRAM_MARGINS, population_records_));
}

Environment::Environment(std::vector<Creature> particles) { // For testing.
    width_ = DEFAULT_WIDTH;
    height_ = DEFAULT_HEIGHT;
    x_coor_ = DEFAULT_X_COOR;
    y_coor_ = DEFAULT_Y_COOR;
}

Environment::Environment(size_t width, size_t height, int x_coor, int y_coor, std::vector<Creature> particles) { // For testing.
    width_ = width;
    height_ = height;
    x_coor_ = x_coor;
    y_coor_ = y_coor;
}

void Environment::Display() const {
  // Displays Title.
  ci::gl::drawStringCentered("Natural Selection", vec2(x_coor_ + (width_ / 2), y_coor_ - 90),
                             ci::Color("white"), ci::Font("Arial", DEFAULT_TITLE_FONT_SIZE));

  ci::gl::drawStringCentered("Speed Count: " + std::to_string(GetSpeedCount()) +
                                ", Intelligence Count: " + std::to_string(GetIntelligenceCount()) +
                                ", Both Count: " + std::to_string(GetBothTypeCount()) +
                             ", Food Count: " + std::to_string(food_.size()) +
                             ", Trials Run: " + std::to_string(population_records_.size() - 1),
                             vec2(x_coor_ + (width_ / 2), y_coor_ - 55),
                               ci::Color("white"), ci::Font("Arial", DEFAULT_SMALL_FONT_SIZE));

  ci::gl::drawString(std::string("Welcome to the Natural Selection Simulator. We have two creature types: speed and intelligence. \n") +
                    "Both traits have an energy cost change for either change in trait. " + "Press 0 to introduce speed creatures \n" +
                    "and press 1 to introduce intelligent creatures. Press the up and down arrows to adjust the amount of food \n" +
                    "resources within the environment. Press enter to simulate each generation.",
                    vec2(1000, y_coor_), ci::Color("white"), ci::Font("Arial", DEFAULT_SMALL_FONT_SIZE));

  // Displays all speed creatures.
  for (size_t i = 0; i < creatures_.size(); i++) {
      Creature curr_particle = creatures_.at(i);
      ci::gl::color(curr_particle.GetColor());
      ci::gl::drawSolidCircle(curr_particle.GetPosition(), curr_particle.GetRadius());
      ci::gl::drawStrokedCircle(curr_particle.GetPosition(),
                                (float) curr_particle.GetVisionRadius(), 0.2f);
  }

  // Displays food particles.
  for (size_t i = 0; i < food_.size(); i++) {
      Food curr_particle = food_.at(i);
      ci::gl::color(curr_particle.GetColor());
      ci::gl::drawSolidCircle(curr_particle.GetPosition(), curr_particle.GetRadius());
  }

  // Displays borders
  ci::gl::color(ci::Color("white"));
  ci::gl::drawStrokedRect(ci::Rectf(vec2(x_coor_, y_coor_),
                                    vec2(x_coor_ + width_, y_coor_ + height_)));

  // Displays histograms.
  for (size_t i = 0; i < speed_histograms_.size(); i++) {
      speed_histograms_.at(i).PrintGraph();
  }

  for (size_t i = 0; i < intelligence_histograms_.size(); i++) {
      intelligence_histograms_.at(i).PrintGraph();
  }

  for (size_t i = 0; i < scatter_plots_.size(); i++) {
      scatter_plots_.at(i).PrintGraph();
  }

  for (size_t i = 0; i < population_graphs_.size(); i++) {
      population_graphs_.at(i).PrintGraph();
  }
}

void Environment::AdvanceOneFrame() {
  if (needs_reset) {
      // Function to spawn more creatures based on replicating creatures.
      KillAndReproduceSpeedCreatures();

      if (!ContainsSpeedCreatures()) {
          RemoveSpeedCreatures();
      }

      if (!ContainsIntelligenceCreatures()) {
          RemoveIntelligenceCreatures();
      }

      if (!ContainsBothTypeCreatures()) {
          RemoveBothTypeCreatures();
      }

      // Reset Food and Creature Energies, Velocities, and Food
      Creature::ResetAllCreatures(creatures_);
      RefreshFood();

      needs_reset = false;
      for (size_t i = 0; i < speed_histograms_.size(); i++) {
          speed_histograms_.at(i).SetParticles(creatures_);
      }

      for (size_t i = 0; i < intelligence_histograms_.size(); i++) {
          intelligence_histograms_.at(i).SetParticles(creatures_);
      }

      for (size_t i = 0; i < scatter_plots_.size(); i++) {
          scatter_plots_.at(i).SetParticles(creatures_);
      }

      population_records_.push_back(creatures_);
      for (size_t i = 0; i < population_graphs_.size(); i++) {
          population_graphs_.at(i).SetParticles(population_records_);
      }
  }

  if (AreAllParticlesReturned()) {
      is_running_ = false;
      needs_reset = true;
  }

  if (is_running_) {
      // LOOP THROUGH ALL SPEED CREATURES
      for (size_t i = 0; i < creatures_.size(); i++) {
          Creature &curr_creature = creatures_.at(i);
          if (curr_creature.GetNeedsMovement()) {
              curr_creature.ChangeVelocityTowardsFurthestCorner();
              curr_creature.SetNeedsMovement(false);
          }

          if (!food_.empty()) {
              for (size_t i = 0; i < food_.size(); i++) {
                  if (curr_creature.IsTouchingSpecificFood(food_.at(i))) {
                      if (curr_creature.GetFood() < 2) {
                          curr_creature.AddFood();
                          food_.erase(food_.begin() + i);
                          curr_creature.SetNeedsMovement(true);
                      }
                  }
              }

              if (!food_.empty()) {
                  for (size_t i = 0; i < creatures_.size(); i++) {
                      if (curr_creature.ChangeVelocityTowardsNearestFood(food_)) {
                          curr_creature.SetNeedsMovement(true);
                      }
                  }
              }

              curr_creature.ChangeVelocityIfNotEnoughEnergy();
          } else { // If no food, then creatures should all return home
              curr_creature.SetNeedsMovement(false);
              curr_creature.ChangeVelocityTowardsNearestWall();
              DetectSpeedCreatureWallHits(curr_creature);
          }

          if (curr_creature.GetFood() == 2) {
              curr_creature.SetNeedsMovement(false);
          }
          curr_creature.ChangeVelocityIfEnoughFood(); // Should go home if has two food.
          DetectSpeedCreatureWallHits(curr_creature);
          // Update all particle positions.
          creatures_.at(i).Move();
      }
  }
}

void Environment::AddSpeedCreatures() {
    // Spawn speed creatures //
    std::vector<Creature> speed_creatures = Creature::SpawnCreatures(SPEED,
                                                                     DEFAULT_COUNT, ci::Color("Red"), DEFAULT_CREATURE_RADIUS, DEFAULT_CREATURE_MASS,
                                                                     DEFAULT_ENERGY_CAPACITY, 0);
    creatures_.insert(creatures_.end(), speed_creatures.begin(), speed_creatures.end());

    // Histogram Data
    speed_histograms_.push_back(SpeedHistogram("Red", DEFAULT_HISTOGRAM_WIDTH,
                                               DEFAULT_HISTOGRAM_HEIGHT, DEFAULT_X_COOR,
                                         DEFAULT_HEIGHT + DEFAULT_Y_COOR + DEFAULT_HISTOGRAM_MARGINS, creatures_));
}

void Environment::RemoveSpeedCreatures() {
    std::vector<Creature> new_creature_vector;
    for (size_t i = 0; i < creatures_.size(); i++) {
        if (creatures_.at(i).GetCreatureType() != SPEED) {
            new_creature_vector.push_back(creatures_.at(i));
        }
    }

    creatures_ = new_creature_vector;
    speed_histograms_.clear();
}

void Environment::AddIntelligenceCreatures() {
    // Spawn intelligence creatures //
    std::vector<Creature> intelligence_creatures = Creature::SpawnCreatures(INTELLIGENCE,
                                                                            DEFAULT_COUNT, ci::Color("Blue"), DEFAULT_CREATURE_RADIUS, DEFAULT_CREATURE_MASS,
                                                                            DEFAULT_ENERGY_CAPACITY, 0);
    creatures_.insert(creatures_.end(), intelligence_creatures.begin(), intelligence_creatures.end());

    // Histogram Data
    intelligence_histograms_.push_back(IntelligenceHistogram("Blue", DEFAULT_HISTOGRAM_WIDTH,
                                                             DEFAULT_HISTOGRAM_HEIGHT, DEFAULT_X_COOR * 3 + DEFAULT_HISTOGRAM_WIDTH + DEFAULT_HISTOGRAM_MARGINS,
                                                             DEFAULT_HEIGHT + DEFAULT_Y_COOR + DEFAULT_HISTOGRAM_MARGINS, creatures_));
}

void Environment::RemoveIntelligenceCreatures() {
    std::vector<Creature> new_creature_vector;
    for (size_t i = 0; i < creatures_.size(); i++) {
        if (creatures_.at(i).GetCreatureType() != INTELLIGENCE) {
            new_creature_vector.push_back(creatures_.at(i));
        }
    }

    creatures_ = new_creature_vector;
    intelligence_histograms_.clear();
}

void Environment::AddBothTypeCreatures() {
    // Spawn intelligence creatures //
    std::vector<Creature> both_type_creatures = Creature::SpawnCreatures(BOTH,
                                                                         DEFAULT_COUNT, ci::Color(ci::Color(0.8f, 0.0f, 0.8f)),
                                                                         DEFAULT_CREATURE_RADIUS, DEFAULT_CREATURE_MASS,
                                                                         DEFAULT_ENERGY_CAPACITY, 0);
    creatures_.insert(creatures_.end(), both_type_creatures.begin(), both_type_creatures.end());

    // Histogram Data
    scatter_plots_.push_back(BothScatterPlot("Purple", DEFAULT_HISTOGRAM_WIDTH * 2,
                                               DEFAULT_HISTOGRAM_HEIGHT * 2, 1000 + DEFAULT_HISTOGRAM_WIDTH * 2 + DEFAULT_HISTOGRAM_MARGINS,
                                             DEFAULT_Y_COOR * 2 + DEFAULT_HISTOGRAM_MARGINS, creatures_));
}

void Environment::RemoveBothTypeCreatures() {
    std::vector<Creature> new_creature_vector;
    for (size_t i = 0; i < creatures_.size(); i++) {
        if (creatures_.at(i).GetCreatureType() != BOTH) {
            new_creature_vector.push_back(creatures_.at(i));
        }
    }

    creatures_ = new_creature_vector;
    scatter_plots_.clear();
}

bool Environment::ContainsSpeedCreatures() {
    for (size_t i = 0; i < creatures_.size(); i++) {
        if (creatures_.at(i).GetCreatureType() == SPEED) {
            return true;
        }
    }

    return false;
}

bool Environment::ContainsIntelligenceCreatures() {
    for (size_t i = 0; i < creatures_.size(); i++) {
        if (creatures_.at(i).GetCreatureType() == INTELLIGENCE) {
            return true;
        }
    }

    return false;
}

bool Environment::ContainsBothTypeCreatures() {
    for (size_t i = 0; i < creatures_.size(); i++) {
        if (creatures_.at(i).GetCreatureType() == BOTH) {
            return true;
        }
    }

    return false;
}

std::vector<Food> Environment::GetFood() {
    return food_;
}

void Environment::DetectSpeedCreatureWallHits(Creature& curr_creature) const {
    float x_pos = curr_creature.GetPosition().x;
    float y_pos = curr_creature.GetPosition().y;

    /* --- SIDE WALLS --- */
    if (x_pos <= x_coor_) { // Hits left wall
        if (curr_creature.GetEnergy() <= 0 || curr_creature.GetFood() == 2 || food_.empty()) {
            curr_creature.SetVelocity(vec2(0,0));
        } else if (curr_creature.GetVelocity().x < 0) { // Checks that the particle is moving towards wall
            curr_creature.SetVelocity(Physics::GetFlippedXVelocity(curr_creature));
            //curr_creature.ChangeVelocityTowardsFurthestCorner();
        }
    }

    if (x_pos >= (x_coor_ + width_)) { // Hits right wall
        if (curr_creature.GetEnergy() <= 0 || curr_creature.GetFood() == 2 || food_.empty()) {
            curr_creature.SetVelocity(vec2(0,0));
        } else if (curr_creature.GetVelocity().x > 0) { // Checks that the particle is moving towards wall
            curr_creature.SetVelocity(Physics::GetFlippedXVelocity(curr_creature));
            //curr_creature.ChangeVelocityTowardsFurthestCorner();
        }
    }

    /* --- TOP AND BOTTOM WALLS --- */
    if (y_pos <= y_coor_) { // Hits top wall
        if (curr_creature.GetEnergy() <= 0 || curr_creature.GetFood() == 2 || food_.empty()) {
            curr_creature.SetVelocity(vec2(0,0));
        } else if (curr_creature.GetVelocity().y < 0) { // Checks that the particle is moving towards wall
            curr_creature.SetVelocity(Physics::GetFlippedYVelocity(curr_creature));
            //curr_creature.ChangeVelocityTowardsFurthestCorner();
        }
    }

    if (y_pos >= (y_coor_ + height_)) { // Hits bottom wall
        if (curr_creature.GetEnergy() <= 0 || curr_creature.GetFood() == 2 || food_.empty()) {
            curr_creature.SetVelocity(vec2(0,0));
        } else if (curr_creature.GetVelocity().y > 0) { // Checks that the particle is moving towards wall
            curr_creature.SetVelocity(Physics::GetFlippedYVelocity(curr_creature));
            //curr_creature.ChangeVelocityTowardsFurthestCorner();
        }
    }
}

bool Environment::AreAllParticlesReturned() {
    if (food_.empty()) {
        return true;
    } else {
        bool all_touching_side = true;
        for (size_t i = 0; i < creatures_.size(); i++) {
            Creature &curr_creature = creatures_.at(i);
            float x_pos = curr_creature.GetPosition().x;
            float y_pos = curr_creature.GetPosition().y;

            if (creatures_.at(i).GetEnergy() > 0.0 && creatures_.at(i).GetFood() < 2) {
                return false;
            }

            /* --- SIDE WALLS --- */
            if (!(x_pos <= x_coor_ || x_pos >= (x_coor_ + width_)
                  || y_pos <= y_coor_ || y_pos >= (y_coor_ + height_))) { // IF NOT TOUCHING ANY WALL
                all_touching_side = false;
            }
        }

        if (!all_touching_side) {
            return false;
        } else {
            return true;
        }
    }
}

std::vector<Creature> Environment::GetSpeedCreatures() {
    return creatures_;
}

bool Environment::GetIsRunning() const {
    return is_running_;
}

void Environment::SetIsRunning(bool setter) {
    is_running_ = setter;
}

void Environment::KillAndReproduceSpeedCreatures() {
    std::vector<Creature> creatures_to_keep;
    for (size_t i = 0; i < creatures_.size(); i++) {
        if (creatures_.at(i).GetFood() > 0) { // Remove dead creatures
            creatures_to_keep.push_back(creatures_.at(i));
        }
    }

    std::vector<Creature> creatures_to_add;
    for (size_t i = 0; i < creatures_to_keep.size(); i++) { // Spawn new creatures based on surviving ones
        // new speeds
        if (creatures_to_keep.at(i).GetFood() > 1) {
            int type = creatures_to_keep.at(i).GetCreatureType();
            Creature child = creatures_to_keep.at(i).CreateChild(type);
            creatures_to_add.push_back(child);
        }
    }

    creatures_to_keep.insert(creatures_to_keep.end(), creatures_to_add.begin(), creatures_to_add.end());
    creatures_ = creatures_to_keep;
}

void Environment::IncreaseFoodCount() {
    food_count_++;
}

void Environment::DecreaseFoodCount() {
    if (food_count_ > 1) {
        food_count_--;
    }
}

void Environment::RefreshFood() {
    food_ = Food::SpawnParticles(food_count_, ci::Color("Green"), 2.0f, 20);
}

bool Environment::AreThereCreaturesAlive() {
    return !creatures_.empty();
}

int Environment::GetSpeedCount() const {
    int sum = 0;
    for (size_t i = 0; i < creatures_.size(); i++) {
        if (creatures_.at(i).GetCreatureType() == SPEED) {
            sum++;
        }
    }

    return sum;
}

int Environment::GetIntelligenceCount() const {
    int sum = 0;
    for (size_t i = 0; i < creatures_.size(); i++) {
        if (creatures_.at(i).GetCreatureType() == INTELLIGENCE) {
            sum++;
        }
    }

    return sum;
}

int Environment::GetBothTypeCount() const {
    int sum = 0;
    for (size_t i = 0; i < creatures_.size(); i++) {
        if (creatures_.at(i).GetCreatureType() == BOTH) {
            sum++;
        }
    }

    return sum;
}


}  // namespace naturalselection
