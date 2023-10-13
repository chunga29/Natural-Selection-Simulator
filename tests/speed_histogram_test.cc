#include <catch2/catch.hpp>

#include <environment.h>
#include <creature.h>
#include <speed_histogram.h>
#include <physics.h>

using naturalselection::Environment;
using naturalselection::Creature;
using naturalselection::SpeedHistogram;
using naturalselection::Physics;

TEST_CASE("Calculate Number of Bins Test") {
  std::vector<Creature> particles = Creature::SpawnCreatures(0, 10, ci::Color("white"), 10, 10, 0, 0);
  SpeedHistogram histogram = SpeedHistogram("White", 0, 0, 0, 0, particles);

  REQUIRE(histogram.CalculateNumOfBins() == 4);
}

TEST_CASE("Calculate Bin Width Test") {
    std::vector<Creature> particles;
    Creature particle1 = Creature(SPEED, vec2(6, 50), vec2(2, 0), 5, 10,
                                  ci::Color("blue"), 0, 0);
    particles.push_back(particle1);
    Creature particle2 = Creature(SPEED, vec2(6, 50), vec2(2, 0), 5, 10,
                                  ci::Color("blue"), 0, 0);
    particles.push_back(particle2);
    Creature particle3 = Creature(SPEED, vec2(6, 50), vec2(-1, 0), 5, 10,
                                  ci::Color("blue"), 0, 0);
    particles.push_back(particle3);
    Creature particle4 = Creature(SPEED, vec2(6, 50), vec2(-1, 0), 5, 10,
                                  ci::Color("blue"), 0, 0);
    particles.push_back(particle4);

    SpeedHistogram histogram = SpeedHistogram("White", 0, 0, 0, 0, particles);
    REQUIRE(histogram.CalculateBinWidth(histogram.CalculateNumOfBins()) == 0.5);
}

TEST_CASE("Create Bin Mapping Test") {
    std::vector<Creature> particles;
    Creature particle1 = Creature(SPEED, vec2(6, 50), vec2(2, 0), 5, 10,
                                  ci::Color("blue"), 0, 0);
    particles.push_back(particle1);
    Creature particle2 = Creature(SPEED, vec2(6, 50), vec2(2, 0), 5, 10,
                                  ci::Color("blue"), 0, 0);
    particles.push_back(particle2);
    Creature particle3 = Creature(SPEED, vec2(6, 50), vec2(-1, 0), 5, 10,
                                  ci::Color("blue"), 0, 0);
    particles.push_back(particle3);
    Creature particle4 = Creature(SPEED, vec2(6, 50), vec2(-1, 0), 5, 10,
                                  ci::Color("blue"), 0, 0);
    particles.push_back(particle4);

    SpeedHistogram histogram = SpeedHistogram("White", 0, 0, 0, 0, particles);

    std::map<int, int> bins = histogram.CreateBinMapping();
    REQUIRE(bins.size() == 2);
    REQUIRE(bins.at(0) == 2);
    REQUIRE(bins.at(1) == 2);
}

TEST_CASE("Large Population (1000) Bin Mapping Test") {
    std::vector<Creature> particles = Creature::SpawnCreatures(0, 1000, ci::Color("white"), 10, 10, 0, 0);
    SpeedHistogram histogram = SpeedHistogram("White", 0, 0, 0, 0, particles);

    std::map<int, int> bins = histogram.CreateBinMapping();

    int num_bins = histogram.CalculateNumOfBins();
    float bin_width = histogram.CalculateBinWidth(num_bins);
    float slowest = Physics::FindLowestSpeed(particles);

    int count = 0;
    bool flag = true;

    // Check all bins to see if the correct amount particles are in each one
    // by checking all the particle speeds and comparing against the bounds of
    // each bin.
    for (int i = 0; i < num_bins; i++) {
        count = 0;
        for (size_t j = 0; j < particles.size(); j++) {
            float speed = glm::length(particles.at(j).GetVelocity());
            float lower_bound = (slowest + (bin_width * i));
            float upper_bound = (slowest + (bin_width * (i + 1)));
            if (speed == upper_bound && i == num_bins - 1) { // Accounts for highest velocity.
                count++;
            } else if (speed >= lower_bound && speed < upper_bound) { // Checks if between bounds.
                count++;
            }
        }

        if (bins.find(i) != bins.end()) { // If bin has any particles,
            if (count != bins.at(i)) { // If count does not match value of bin,
                flag = false; // Show false.
            }
        } else { // If the bin does not exist,
            if (count > 0) { // but the count is greater than 1,
                flag = false; // Show false.
            }
        }
    }

    REQUIRE(flag);
}