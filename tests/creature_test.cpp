#include <catch2/catch.hpp>

#include <environment.h>
#include <creature.h>
#include <speed_histogram.h>
#include <physics.h>

using naturalselection::Environment;
using naturalselection::Creature;
using naturalselection::SpeedHistogram;
using naturalselection::Physics;

TEST_CASE("Spawning Speed Children Test") {
    for (size_t i = 0; i < 100; i++) {
        Creature speed_creature = Creature(SPEED, vec2(0, 0), vec2(0, 0), 5,
                                           10, ci::Color("red"), 0.0, 0,
                                           2.5, 0.25);

        Creature child = speed_creature.CreateChild(SPEED);
        bool flag = child.GetMaxVelocity() < 2.75 || child.GetMaxVelocity() > 2.25;
        REQUIRE(flag);
    }
}

TEST_CASE("Spawning Intelligence Children Test") {
    for (size_t i = 0; i < 100; i++) {
        Creature speed_creature = Creature(INTELLIGENCE, vec2(0, 0), vec2(0, 0), 5,
                                           10, ci::Color("red"), 0.0, 0,
                                           2.5, 0.25);

        Creature child = speed_creature.CreateChild(INTELLIGENCE);
        bool flag = child.GetVisionRadius() < 3.75 || child.GetVisionRadius() > 1.25;
        REQUIRE(flag);
    }
}

TEST_CASE("Spawning Both Type Children Test") {
    for (size_t i = 0; i < 100; i++) {
        Creature speed_creature = Creature(BOTH, vec2(0, 0), vec2(0, 0), 5,
                                           10, ci::Color("red"), 0.0, 0,
                                           2.5, 0.25);

        Creature child = speed_creature.CreateChild(BOTH);
        bool flag = (child.GetMaxVelocity() < 2.75 || child.GetMaxVelocity() > 2.25) &&
                (child.GetVisionRadius() < 3.75 || child.GetVisionRadius() > 1.25);
        REQUIRE(flag);
    }
}

TEST_CASE("Energy Cost Speed Test") {
    for (size_t i = 0; i < 100; i++) {
        Creature speed_creature = Creature(SPEED, vec2(0, 0), vec2(0, 0), 5,
                                           10, ci::Color("red"), 0.0, 0,
                                           2.5, 0.25);

        Creature child = speed_creature.CreateChild(SPEED);

        if (child.GetMaxVelocity() > 2.5) {
            REQUIRE(child.GetEnergySpend() > 0.25);
        } else if (child.GetMaxVelocity() < 2.5) {
            REQUIRE(child.GetEnergySpend() < 0.25);
        } else {
            REQUIRE(child.GetEnergySpend() == 0.25);
        }
    }
}

TEST_CASE("Energy Cost Intelligence Test") {
    for (size_t i = 0; i < 100; i++) {
        Creature speed_creature = Creature(INTELLIGENCE, vec2(0, 0), vec2(0, 0), 5,
                                           10, ci::Color("red"), 0.0, 0,
                                           2.5, 0.25);

        Creature child = speed_creature.CreateChild(INTELLIGENCE);

        if (child.GetVisionRadius() > 12) {
            REQUIRE(child.GetEnergySpend() > 0.25);
        } else if (child.GetVisionRadius() < 12) {
            REQUIRE(child.GetEnergySpend() < 0.25);
        } else {
            REQUIRE(child.GetEnergySpend() == 0.25);
        }
    }
}