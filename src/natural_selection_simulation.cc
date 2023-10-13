#include "natural_selection_simulation.h"

namespace naturalselection {

NaturalSelectionSimulation::NaturalSelectionSimulation() {
  ci::app::setWindowSize(kWindowSize + kWindowSize, kWindowSize); // 1000 x 2000
}

void NaturalSelectionSimulation::draw() {
  ci::Color background_color("black");
  ci::gl::clear(background_color);
  // Comment
  environment_.Display();
}

void NaturalSelectionSimulation::update() {
    if (environment_.AreThereCreaturesAlive()) {
        environment_.AdvanceOneFrame();
    }
}

void NaturalSelectionSimulation::keyDown(ci::app::KeyEvent event) {
    switch (event.getCode()) {
        case ci::app::KeyEvent::KEY_RETURN:
            if (environment_.ContainsIntelligenceCreatures() ||
                environment_.ContainsSpeedCreatures() ||
                environment_.ContainsBothTypeCreatures()) {
                environment_.SetIsRunning(true);
            }
            break;

        case ci::app::KeyEvent::KEY_DELETE:
            cinder::app::AppMsw::quit();
            break;

        case ci::app::KeyEvent::KEY_UP:
            if (!environment_.GetIsRunning()) {
                environment_.IncreaseFoodCount();
                environment_.RefreshFood();
            }
            break;

        case ci::app::KeyEvent::KEY_DOWN:
            if (!environment_.GetIsRunning()) {
                environment_.DecreaseFoodCount();
                environment_.RefreshFood();
            }
            break;

        case ci::app::KeyEvent::KEY_0:
            if (!environment_.GetIsRunning()) {
                if (environment_.ContainsSpeedCreatures()) {
                    environment_.RemoveSpeedCreatures();
                } else {
                    environment_.AddSpeedCreatures();
                }
            }
            break;

        case ci::app::KeyEvent::KEY_1:
            if (!environment_.GetIsRunning()) {
                if (environment_.ContainsIntelligenceCreatures()) {
                    environment_.RemoveIntelligenceCreatures();
                } else {
                    environment_.AddIntelligenceCreatures();
                }
            }
            break;

        case ci::app::KeyEvent::KEY_2:
            if (!environment_.GetIsRunning()) {
                if (environment_.ContainsBothTypeCreatures()) {
                    environment_.RemoveBothTypeCreatures();
                } else {
                    environment_.AddBothTypeCreatures();
                }
            }
            break;
    }
}

}  // namespace naturalselection
