#include "both_scatter_plot.h"
#include <map>

namespace naturalselection {

using glm::vec2;

BothScatterPlot::BothScatterPlot() {}

BothScatterPlot::BothScatterPlot(std::string color, size_t width, size_t height, int x_coor, int y_coor, std::vector<Creature> particles) {
    color_ = color;
    width_ = width; //   200
    height_ = height; // 200
    x_coor_ = x_coor;
    y_coor_ = y_coor;

    std::vector<Creature> intelligence_creatures;
    for (size_t i = 0; i < particles.size(); i++) {
        if (particles.at(i).GetCreatureType() == BOTH) {
            intelligence_creatures.push_back(particles.at(i));
        }
    }

    creatures_ = intelligence_creatures;
}

void BothScatterPlot::PrintGraph() const {
//    /* --- DRAW CALLS --- */

    // DRAW DOTS
    for (size_t i = 0; i < creatures_.size(); i++) {
        ci::gl::color(ci::Color("Purple"));
        double x_value = GetSpeedCoordinate(const_cast<Creature &>(creatures_.at(i)));
        double y_value = GetVisionCoordinate(const_cast<Creature &>(creatures_.at(i)));

        ci::gl::drawSolidCircle(vec2(x_value + x_coor_, y_value + y_coor_), 2);
    }

    ci::gl::color(ci::Color("white"));
    ci::gl::drawStrokedCircle(vec2(width_ / 2 + x_coor_, height_ / 2 + y_coor_), 10);
    // Draw histogram box.
    ci::gl::drawStrokedRect(ci::Rectf(vec2(x_coor_, y_coor_),
                                      vec2(x_coor_ + width_, y_coor_ + height_)));

    // Draw title.
    std::string title = "Both Trait Scatter Plot";
    ci::gl::drawStringCentered(title, vec2(x_coor_ + (width_ / 2), y_coor_ - DEFAULT_MARGIN_SIZE),
                               ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));

    // Draw x-axis label.
    ci::gl::drawStringCentered("Speed", vec2(x_coor_ + (width_ / 2), y_coor_ + height_ + DEFAULT_MARGIN_SIZE),
                               ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));

    // Draw y-axis label.
    ci::gl::drawStringCentered(std::string("R\na\nd\ni\nu\ns"), vec2(x_coor_ - 20, y_coor_ + height_ / 2 - DEFAULT_MARGIN_SIZE),
                               ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));

    std::string avg_radius = "Average Radius: " + FloatToStringPrecision((float) GetAverageVisionRadius(), 3);
    ci::gl::drawStringCentered(avg_radius, vec2(x_coor_ + (width_ / 2), y_coor_ + height_ + DEFAULT_MARGIN_SIZE + DEFAULT_MARGIN_SIZE),
                               ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));

    std::string avg_speed = "Average Speed: " + FloatToStringPrecision((float) GetAverageSpeed(), 3);
    ci::gl::drawStringCentered(avg_speed, vec2(x_coor_ + (width_ / 2), y_coor_ + height_ + DEFAULT_MARGIN_SIZE * 3),
                               ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));
}

void BothScatterPlot::SetParticles(std::vector<Creature> new_particles) {
    std::vector<Creature> intelligence_creatures;
    for (size_t i = 0; i < new_particles.size(); i++) {
        if (new_particles.at(i).GetCreatureType() == BOTH) {
            intelligence_creatures.push_back(new_particles.at(i));
        }
    }

    creatures_ = intelligence_creatures;
}

// Source: https://www.codegrepper.com/code-examples/cpp/c%2B%2B+round+float+to+2+decimal+places
std::string BothScatterPlot::FloatToStringPrecision(float value, unsigned char prec) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(prec) << value;
    std::string new_string = ss.str();
    return new_string;
}

double BothScatterPlot::GetAverageVisionRadius() const {
    double sum = 0.0;
    for (size_t i = 0; i < creatures_.size(); i++) {
        sum += creatures_.at(i).GetVisionRadius();
    }

    return sum / creatures_.size();
}

double BothScatterPlot::GetLargestVisionRadius() const {
    double max = 0.0;
    for (size_t i = 0; i < creatures_.size(); i++) {
        if (max < creatures_.at(i).GetVisionRadius()) {
            max = creatures_.at(i).GetVisionRadius();
        }
    }

    return max;
}

double BothScatterPlot::GetSmallestVisionRadius() const {
    double max = std::numeric_limits<double>::infinity();
    for (size_t i = 0; i < creatures_.size(); i++) {
        if (max > creatures_.at(i).GetVisionRadius()) {
            max = creatures_.at(i).GetVisionRadius();
        }
    }

    return max;
}

float BothScatterPlot::GetAverageSpeed() const {
    float sum = 0.0f;
    for (size_t i = 0; i < creatures_.size(); i++) {
        sum += creatures_.at(i).GetMaxVelocity();
    }

    return sum / creatures_.size();
}

double BothScatterPlot::GetVisionCoordinate(Creature& curr_creature) const {
    double percent_change = (curr_creature.GetVisionRadius() / 12) - 1;

    double coordinate = (0.5 * (width_)) + (width_ * (percent_change)) / 2;
    return coordinate;
}

double BothScatterPlot::GetSpeedCoordinate(Creature& curr_creature) const {
    double percent_change = (curr_creature.GetMaxVelocity() / 2.5) - 1;

    double coordinate = (0.5 * (width_)) + (width_ * (percent_change)) / 4;
    return coordinate;
}

}