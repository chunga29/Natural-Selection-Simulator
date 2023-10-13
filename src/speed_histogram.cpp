#include "speed_histogram.h"
#include <map>

namespace naturalselection {

using glm::vec2;

SpeedHistogram::SpeedHistogram() {}

SpeedHistogram::SpeedHistogram(std::string color, size_t width, size_t height, int x_coor, int y_coor, std::vector<Creature> particles) {
    color_ = color;
    width_ = width; //   200
    height_ = height; // 200
    x_coor_ = x_coor;
    y_coor_ = y_coor;

    std::vector<Creature> speed_creatures;
    for (size_t i = 0; i < particles.size(); i++) {
        if (particles.at(i).GetCreatureType() == SPEED) {
            speed_creatures.push_back(particles.at(i));
        }
    }

    creatures_ = speed_creatures;
}

void SpeedHistogram::PrintVerticalBars(std::map<int, int> bins) const {
    size_t num_particles = creatures_.size();
    int num_bins = CalculateNumOfBins();
    float bin_width = CalculateBinWidth(num_bins);

    float bin_pixel_width = (float) width_ / num_bins;
    float bin_pixel_height_unit = (float) height_ / num_particles; // Divided by half the amount of particles

    // Draw the markers under first vertical line.
    float slowest = Physics::FindLowestSpeed(creatures_);

    std::string slowest_string = FloatToStringPrecision(slowest, 2);
    ci::gl::drawStringCentered(slowest_string,
                               vec2(x_coor_,
                                    y_coor_ + height_ + DEFAULT_MARGIN_SIZE / 2),
                               ci::Color("white"), ci::Font("Arial", DEFAULT_SMALL_FONT_SIZE));

    for (size_t i = 0; i < (size_t) num_bins; i++) {
        ci::gl::color(ci::Color(color_.c_str()));
        if (bins.find(i) != bins.end()) { // If there are values in the bin, draw the bin bar.
            ci::gl::drawSolidRect(ci::Rectf(vec2(x_coor_ + (bin_pixel_width * i),
                                                 y_coor_ + height_ - (bin_pixel_height_unit * bins.at(i))),
                                            vec2(x_coor_ + (bin_pixel_width * (i + 1)),
                                                 y_coor_ + height_)));
        }

        // Draw the vertical line on the right side of each bar.
        ci::gl::color(ci::Color("white"));
        ci::gl::drawLine(vec2(x_coor_ + (bin_pixel_width * (i + 1)),
                              y_coor_),
                         vec2(x_coor_ + (bin_pixel_width * (i + 1)),
                              y_coor_ + height_));

        // Draw the markers under vertical lines.
        std::string marker_string = FloatToStringPrecision(slowest + bin_width * (i + 1), 2);
        ci::gl::drawStringCentered(marker_string,
                                   vec2(x_coor_ + (bin_pixel_width * (i + 1)),
                                        y_coor_ + height_ + DEFAULT_MARGIN_SIZE / 2),
                                   ci::Color("white"), ci::Font("Arial", DEFAULT_SMALL_FONT_SIZE));
    }
}

void SpeedHistogram::PrintNumericalLines() const {
    size_t num_particles = creatures_.size();

    // Three quarter marker.
    double three_quarter_mark = ((double) num_particles / 4) * 3;
    if (floor(three_quarter_mark) == three_quarter_mark) { // Check if whole number...
        ci::gl::drawLine(vec2(x_coor_, y_coor_ + (height_ / 4)), vec2(x_coor_ + width_, y_coor_ + (height_ / 4)));
        ci::gl::drawStringCentered(std::to_string((int) three_quarter_mark), vec2(x_coor_ - DEFAULT_MARGIN_SIZE, y_coor_ + (height_ / 4)),
                                   ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));
    }

    // Half way marker.
    double half_way_mark = ((double) num_particles / 4) * 2;
    if (floor(half_way_mark) == half_way_mark) { // Check if whole number...
        ci::gl::drawLine(vec2(x_coor_, y_coor_ + (height_ / 2)), vec2(x_coor_ + width_, y_coor_ + (height_ / 2)));
        ci::gl::drawStringCentered(std::to_string((int) half_way_mark), vec2(x_coor_ - DEFAULT_MARGIN_SIZE, y_coor_ + (height_ / 2)),
                                   ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));
    }

    // One quarter marker.
    double one_quarter_mark = (double) num_particles / 4;
    if (floor(one_quarter_mark) == one_quarter_mark) { // Check if whole number...
        ci::gl::drawLine(vec2(x_coor_, y_coor_ + 3 * (height_ / 4)),
                         vec2(x_coor_ + width_, y_coor_ + 3 * (height_ / 4)));
        ci::gl::drawStringCentered(std::to_string((int) one_quarter_mark), vec2(x_coor_ - DEFAULT_MARGIN_SIZE, y_coor_ + 3 * (height_ / 4)),
                                   ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));
    }
}

void SpeedHistogram::PrintGraph() const {
    // This map will keep track of each bin number, starting from index 0.
    // Each map key will represent a bin and each value that is connected
    // to the key represents the amount of particles within each bin.
    std::map<int, int> bins = CreateBinMapping();

    /* --- DRAW CALLS --- */
    // Draw histogram bars.
    PrintVerticalBars(bins);

    // Draw histogram box.
    ci::gl::drawStrokedRect(ci::Rectf(vec2(x_coor_, y_coor_),
                                      vec2(x_coor_ + width_, y_coor_ + height_)));

    // Draw title.
    std::string title = "Creature Speed Histogram";
    ci::gl::drawStringCentered(title, vec2(x_coor_ + (width_ / 2), y_coor_ - DEFAULT_MARGIN_SIZE),
                               ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));

    // Draw x-axis label.
    ci::gl::drawStringCentered("Speed", vec2(x_coor_ + (width_ / 2), y_coor_ + height_ + DEFAULT_MARGIN_SIZE),
                               ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));

    // Draw numerical lines.
    PrintNumericalLines();

    std::string avg_speed = "Average Speed: " + FloatToStringPrecision((float) GetAverageSpeed(), 3);
    ci::gl::drawStringCentered(avg_speed, vec2(x_coor_ + (width_ / 2), y_coor_ + height_ + DEFAULT_MARGIN_SIZE + DEFAULT_MARGIN_SIZE),
                               ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));
}

std::map<int, int> SpeedHistogram::CreateBinMapping() const {
    std::map<int, int> bins;

    int num_bins = CalculateNumOfBins();
    size_t num_particles = creatures_.size();
    float bin_width = CalculateBinWidth(num_bins);
    float slowest = Physics::FindLowestSpeed(creatures_);

    for (size_t i = 0; i < num_particles; i++) {
        int curr_bin_num = 0;
        for (size_t j = 1; j < (size_t) num_bins; j++) { // Find Which Bin
            float speed = glm::length(creatures_.at(i).GetVelocity());
            float bin_lower_bound = slowest + (j * bin_width);
            if (speed >= bin_lower_bound) {
                // Finds the correct bin by adding the amount of bins already passed to the slowest particle speed.
                // The bin width will add the speed range of a full bin.
                curr_bin_num = j;
            } else {
                break;
            }
        }

        // Source: https://www.techiedelight.com/increment-map-value-associated-with-key-cpp/
        // check if the current bin num exists in the map or not
        auto it = bins.find(curr_bin_num);
        if (it != bins.end()) { // key already present on the map
            it->second++;    // increment map's value for bin num key
        } else { // key not found
            bins.insert(std::make_pair(curr_bin_num, 1));
        }
    }

    return bins;
}

int SpeedHistogram::CalculateNumOfBins() const {
    // Calculates number of bins by taking the square root of the
    // number of particles and taking the nearest upward whole number.
    double bins = sqrt((double) creatures_.size());
    return (int) ceil(bins);
}

float SpeedHistogram::CalculateBinWidth(int num_of_bins) const {
    float fastest = Physics::FindHighestSpeed(creatures_);
    float slowest = Physics::FindLowestSpeed(creatures_);

    // Calculates bin width by dividing the range by the number of bins.
    return (fastest - slowest) / num_of_bins;
}

void SpeedHistogram::SetParticles(std::vector<Creature> new_particles) {
    std::vector<Creature> speed_creatures;
    for (size_t i = 0; i < new_particles.size(); i++) {
        if (new_particles.at(i).GetCreatureType() == SPEED) {
            speed_creatures.push_back(new_particles.at(i));
        }
    }

    creatures_ = speed_creatures;
}

// Source: https://www.codegrepper.com/code-examples/cpp/c%2B%2B+round+float+to+2+decimal+places
std::string SpeedHistogram::FloatToStringPrecision(float value, unsigned char prec) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(prec) << value;
    std::string new_string = ss.str();
    return new_string;
}

float SpeedHistogram::GetAverageSpeed() const {
    float sum = 0.0f;
    for (size_t i = 0; i < creatures_.size(); i++) {
        sum += creatures_.at(i).GetMaxVelocity();
    }

    return sum / creatures_.size();
}

}