#include "intelligence_histogram.h"
#include <map>

namespace naturalselection {

using glm::vec2;

IntelligenceHistogram::IntelligenceHistogram() {}

IntelligenceHistogram::IntelligenceHistogram(std::string color, size_t width, size_t height, int x_coor, int y_coor, std::vector<Creature> particles) {
    color_ = color;
    width_ = width; //   200
    height_ = height; // 200
    x_coor_ = x_coor;
    y_coor_ = y_coor;

    std::vector<Creature> intelligence_creatures;
    for (size_t i = 0; i < particles.size(); i++) {
        if (particles.at(i).GetCreatureType() == INTELLIGENCE) {
            intelligence_creatures.push_back(particles.at(i));
        }
    }

    creatures_ = intelligence_creatures;
}

void IntelligenceHistogram::PrintVerticalBars(std::map<int, int> bins) const {
    size_t num_particles = creatures_.size();
    int num_bins = CalculateNumOfBins();
    float bin_width = CalculateBinWidth(num_bins);

    float bin_pixel_width = (float) width_ / num_bins;
    float bin_pixel_height_unit = (float) height_ / num_particles; // Divided by half the amount of particles

    // Draw the markers under first vertical line.
    float smallest = (float) GetSmallestVisionRadius();

    std::string slowest_string = FloatToStringPrecision(smallest, 2);
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
        std::string marker_string = FloatToStringPrecision(smallest + bin_width * (i + 1), 2);
        ci::gl::drawStringCentered(marker_string,
                                   vec2(x_coor_ + (bin_pixel_width * (i + 1)),
                                        y_coor_ + height_ + DEFAULT_MARGIN_SIZE / 2),
                                   ci::Color("white"), ci::Font("Arial", DEFAULT_SMALL_FONT_SIZE));
    }
}

void IntelligenceHistogram::PrintNumericalLines() const {
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

void IntelligenceHistogram::PrintGraph() const {
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
    std::string title = "Creature Intelligence (Vision Radius) Histogram";
    ci::gl::drawStringCentered(title, vec2(x_coor_ + (width_ / 2), y_coor_ - DEFAULT_MARGIN_SIZE),
                               ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));

    // Draw x-axis label.
    ci::gl::drawStringCentered("Radius", vec2(x_coor_ + (width_ / 2), y_coor_ + height_ + DEFAULT_MARGIN_SIZE),
                               ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));

    // Draw numerical lines.
    PrintNumericalLines();

    std::string avg_radius = "Average Radius: " + FloatToStringPrecision((float) GetAverageVisionRadius(), 3);
    ci::gl::drawStringCentered(avg_radius, vec2(x_coor_ + (width_ / 2), y_coor_ + height_ + DEFAULT_MARGIN_SIZE + DEFAULT_MARGIN_SIZE),
                               ci::Color("white"), ci::Font("Arial", DEFAULT_FONT_SIZE));
}

std::map<int, int> IntelligenceHistogram::CreateBinMapping() const {
    std::map<int, int> bins;

    int num_bins = CalculateNumOfBins();
    size_t num_particles = creatures_.size();
    float bin_width = CalculateBinWidth(num_bins);
    float slowest = (float) GetSmallestVisionRadius();

    for (size_t i = 0; i < num_particles; i++) {
        int curr_bin_num = 0;
        for (size_t j = 1; j < (size_t) num_bins; j++) { // Find Which Bin
            float radius = (float) creatures_.at(i).GetVisionRadius();
            float bin_lower_bound = (float) (slowest + (j * bin_width));
            if (radius >= bin_lower_bound) {
                // Finds the correct bin by adding the amount of bins already passed to the smallest radius.
                // The bin width will add the radius range of a full bin.
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

int IntelligenceHistogram::CalculateNumOfBins() const {
    // Calculates number of bins by taking the square root of the
    // number of particles and taking the nearest upward whole number.
    double bins = sqrt((double) creatures_.size());
    return (int) ceil(bins);
}

float IntelligenceHistogram::CalculateBinWidth(int num_of_bins) const {
    float largest = (float) GetLargestVisionRadius();
    float smallest = (float) GetSmallestVisionRadius();

    // Calculates bin width by dividing the range by the number of bins.
    return (largest - smallest) / num_of_bins;
}

void IntelligenceHistogram::SetParticles(std::vector<Creature> new_particles) {
    std::vector<Creature> intelligence_creatures;
    for (size_t i = 0; i < new_particles.size(); i++) {
        if (new_particles.at(i).GetCreatureType() == INTELLIGENCE) {
            intelligence_creatures.push_back(new_particles.at(i));
        }
    }

    creatures_ = intelligence_creatures;
}

// Source: https://www.codegrepper.com/code-examples/cpp/c%2B%2B+round+float+to+2+decimal+places
std::string IntelligenceHistogram::FloatToStringPrecision(float value, unsigned char prec) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(prec) << value;
    std::string new_string = ss.str();
    return new_string;
}

double IntelligenceHistogram::GetAverageVisionRadius() const {
    double sum = 0.0;
    for (size_t i = 0; i < creatures_.size(); i++) {
        sum += creatures_.at(i).GetVisionRadius();
    }

    return sum / creatures_.size();
}

double IntelligenceHistogram::GetLargestVisionRadius() const {
    double max = 0.0;
    for (size_t i = 0; i < creatures_.size(); i++) {
        if (max < creatures_.at(i).GetVisionRadius()) {
            max = creatures_.at(i).GetVisionRadius();
        }
    }

    return max;
}

double IntelligenceHistogram::GetSmallestVisionRadius() const {
    double max = std::numeric_limits<double>::infinity();
    for (size_t i = 0; i < creatures_.size(); i++) {
        if (max > creatures_.at(i).GetVisionRadius()) {
            max = creatures_.at(i).GetVisionRadius();
        }
    }

    return max;
}

}