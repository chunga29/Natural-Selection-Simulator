#include <catch2/catch.hpp>

#include <environment.h>
#include <creature.h>

using naturalselection::Environment;
using naturalselection::Creature;

const size_t DEFAULT_WIDTH = 700;
const size_t DEFAULT_HEIGHT = 500;
const int DEFAULT_X_COOR = 100;
const int DEFAULT_Y_COOR = 100;
const int MARGIN = 10;

/** --- WALL TESTS --- */

TEST_CASE("Spawning Food within Bounds") {
    Environment environment = Environment();

    for (size_t i = 0 ; i < environment.GetFood().size(); i++) {
        REQUIRE(environment.GetFood().at(i).GetPosition().x <= DEFAULT_X_COOR + DEFAULT_WIDTH - MARGIN);
        REQUIRE(environment.GetFood().at(i).GetPosition().x >= DEFAULT_X_COOR + MARGIN);
        REQUIRE(environment.GetFood().at(i).GetPosition().y <= DEFAULT_Y_COOR + DEFAULT_HEIGHT - MARGIN);
        REQUIRE(environment.GetFood().at(i).GetPosition().y >= DEFAULT_Y_COOR + MARGIN);
    }
}

TEST_CASE("Spawning Creatures on Bounds") {
    Environment environment = Environment();

    for (size_t i = 0 ; i < environment.GetSpeedCreatures().size(); i++) {
        bool require_test = environment.GetSpeedCreatures().at(i).GetPosition().x == DEFAULT_X_COOR + DEFAULT_WIDTH ||
                            environment.GetSpeedCreatures().at(i).GetPosition().x == DEFAULT_X_COOR ||
                            environment.GetSpeedCreatures().at(i).GetPosition().y == DEFAULT_Y_COOR + DEFAULT_HEIGHT ||
                            environment.GetSpeedCreatures().at(i).GetPosition().y == DEFAULT_Y_COOR;
        REQUIRE(require_test);
    }
}