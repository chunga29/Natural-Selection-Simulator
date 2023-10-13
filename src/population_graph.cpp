#include "population_graph.h"
#include <map>

namespace naturalselection {

using glm::vec2;

PopulationGraph::PopulationGraph() {}

PopulationGraph::PopulationGraph(std::string trait, size_t width, size_t height, int x_coor, int y_coor, std::vector<std::vector<Creature>> population_records) {
    trait_ = trait;
    width_ = width; //   200
    height_ = height; // 200
    x_coor_ = x_coor;
    y_coor_ = y_coor;
    population_records_ = population_records;
}

void PopulationGraph::PrintVerticalBars() const {
    size_t num_particles = population_records_.size();
    int num_bins = (int) num_particles;

    float bin_pixel_width = (float) width_ / num_bins;
    float bin_pixel_height_unit = (float) height_ / FindHighestPopulation();

    for (size_t i = 0; i < (size_t) num_bins; i++) {
        ci::gl::color(ci::Color(1.0f, 0.0f, 1.0f));
        ci::gl::drawSolidRect(ci::Rectf(vec2(x_coor_ + (bin_pixel_width * i),
                                             y_coor_ + height_ -
                                             (bin_pixel_height_unit * (GetTotalPopulationAtIndex(i)))),
                                        vec2(x_coor_ + (bin_pixel_width * (i + 1)),
                                             y_coor_ + height_)));

        ci::gl::color(ci::Color("Blue"));
        ci::gl::drawSolidRect(ci::Rectf(vec2(x_coor_ + (bin_pixel_width * i),
                                             y_coor_ + height_ -
                                             (bin_pixel_height_unit * (GetSpeedCountAtIndex(i) +
                                                     GetIntelligenceCountAtIndex(i)))),
                                        vec2(x_coor_ + (bin_pixel_width * (i + 1)),
                                             y_coor_ + height_)));

        ci::gl::color(ci::Color("Red"));
        ci::gl::drawSolidRect(ci::Rectf(vec2(x_coor_ + (bin_pixel_width * i),
                                             y_coor_ + height_ - (bin_pixel_height_unit * GetSpeedCountAtIndex(i))),
                                        vec2(x_coor_ + (bin_pixel_width * (i + 1)),
                                             y_coor_ + height_)));



        if (population_records_.size() < 25) { // If graph has few enough trials, show bars.
            // Draw the vertical line on the right side of each bar.
            ci::gl::color(ci::Color("white"));
            ci::gl::drawLine(vec2(x_coor_ + (bin_pixel_width * (i + 1)),
                                  y_coor_),
                             vec2(x_coor_ + (bin_pixel_width * (i + 1)),
                                  y_coor_ + height_));
        }
    }
}

void PopulationGraph::PrintNumericalLines() const {

}

void PopulationGraph::PrintGraph() const {
//    /* --- DRAW CALLS --- */
    // Draw histogram bars.
    ci::gl::color(ci::Color("White"));
    PrintVerticalBars();

    // Draw histogram box.

    ci::gl::drawStrokedRect(ci::Rectf(vec2(x_coor_, y_coor_),
                                      vec2(x_coor_ + width_, y_coor_ + height_)));

    // Draw title.
    std::string title = "Population";
    ci::gl::drawStringCentered(title, vec2(x_coor_ + (width_ / 2), y_coor_ - DEFAULT_MARGIN_SIZE),
                               ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));

    // Draw x-axis label.
    ci::gl::drawStringCentered("Trials", vec2(x_coor_ + (width_ / 2), y_coor_ + height_ + DEFAULT_MARGIN_SIZE),
                               ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));

    std::string avg_pop = "Average Population: " + FloatToStringPrecision((float) GetAveragePopulation(), 3);
    ci::gl::drawStringCentered(avg_pop, vec2(x_coor_ + (width_ / 2), y_coor_ + height_ + DEFAULT_MARGIN_SIZE + DEFAULT_MARGIN_SIZE),
                               ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));
    // Draw numerical lines.
    PrintNumericalLines();
}

void PopulationGraph::SetParticles(std::vector<std::vector<Creature>> new_particles) {
    population_records_ = new_particles;
}

// Source: https://www.codegrepper.com/code-examples/cpp/c%2B%2B+round+float+to+2+decimal+places
std::string PopulationGraph::FloatToStringPrecision(float value, unsigned char prec) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(prec) << value;
    std::string new_string = ss.str();
    return new_string;
}

size_t PopulationGraph::FindHighestPopulation() const {
    int max = 0;
    for (size_t i = 0; i < population_records_.size(); i++) {
        if (GetTotalPopulationAtIndex(i) > max) {
            max = GetTotalPopulationAtIndex(i);
        }
    }

    return max;
}

double PopulationGraph::GetAveragePopulation() const {
    double sum = 0;
    for (size_t i = 0; i < population_records_.size(); i++) {
        sum += GetTotalPopulationAtIndex(i);
    }

    return (double) sum / (double) population_records_.size();
}

int PopulationGraph::GetSpeedCountAtIndex(int index) const {
    int sum = 0;
    for (size_t i = 0; i < population_records_.at(index).size(); i++) {
        if (population_records_.at(index).at(i).GetCreatureType() == SPEED) {
            sum++;
        }
    }

    return sum;
}

int PopulationGraph::GetIntelligenceCountAtIndex(int index) const {
    int sum = 0;
    for (size_t i = 0; i < population_records_.at(index).size(); i++) {
        if (population_records_.at(index).at(i).GetCreatureType() == INTELLIGENCE) {
            sum++;
        }
    }

    return sum;
}

int PopulationGraph::GetBothTypeCountAtIndex(int index) const {
    int sum = 0;
    for (size_t i = 0; i < population_records_.at(index).size(); i++) {
        if (population_records_.at(index).at(i).GetCreatureType() == BOTH) {
            sum++;
        }
    }

    return sum;
}

int PopulationGraph::GetTotalPopulationAtIndex(int index) const {
    return population_records_.at(index).size();
}

}