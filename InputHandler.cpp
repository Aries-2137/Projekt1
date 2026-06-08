#include "InputHandler.h"
#include "Game.h"
#include <iostream>
#include <cmath>
#include <cctype>

InputHandler::InputHandler() : totalScore(0), lastHit(HitAccuracy::NONE) {}

void InputHandler::handleKeyPress(char key, float currentTimeMs, std::vector<std::unique_ptr<Note>>& activeNotes, Game& game) {
    int targetLane = -1;
    key = std::toupper(key);

    if (key == 'S') targetLane = 1;
    else if (key == 'D') targetLane = 2;
    else if (key == ' ') targetLane = 3;
    else if (key == 'J') targetLane = 4;
    else if (key == 'K') targetLane = 5;

    if (targetLane == -1) return;

    auto closestNoteIt = activeNotes.end();
    float minDiff = 150.0f;

    for (auto it = activeNotes.begin(); it != activeNotes.end(); ++it) {
        if ((*it)->getLane() == targetLane) {
            float diff = std::abs((*it)->getTargetTime() - currentTimeMs);
            if (diff <= minDiff) {
                minDiff = diff;
                closestNoteIt = it;
            }
        }
    }

    if (closestNoteIt != activeNotes.end()) {
        float noteY = (*closestNoteIt)->getY();
        float diff = std::abs((*closestNoteIt)->getTargetTime() - currentTimeMs);

        if (diff <= 150.0f && noteY > 650.0f) {

            int duration = (*closestNoteIt)->getDuration();
            activeNotes.erase(closestNoteIt);
            game.triggerReceptorAnimation(targetLane);

            std::string feedbackMsg;
            sf::Color feedbackColor;

            // --- LOGIKA OCEN ---
            if (diff <= 30.0f) { // - czas na perfect w ms
                totalScore += (lastHit == HitAccuracy::PERFECT) ? 100 : 50;
                lastHit = HitAccuracy::PERFECT; // bonus za ciąg trafień perfektycjnych
                feedbackMsg = "PERFECT!";
                feedbackColor = sf::Color::Cyan;
            }
            else if (diff <= 60.0f) { // - czas na super w ms
                totalScore += (lastHit == HitAccuracy::PERFECT || lastHit == HitAccuracy::SUPER) ? 50 : 20;
                lastHit = HitAccuracy::SUPER; // bonus za ciąg trafień super
                feedbackMsg = "SUPER!";
                feedbackColor = sf::Color::Green;
            }
            else {
                totalScore += 10; // punty za trafienie
                lastHit = HitAccuracy::GOOD;
                feedbackMsg = "GOOD";
                feedbackColor = sf::Color::Yellow;
            }

            // przytrzymanie
            if (duration > 1) {
                laneHoldStartTime[targetLane] = currentTimeMs;
                laneHoldExpectedDuration[targetLane] = (duration - 1) * 250;
            }

            game.addFeedback(targetLane, feedbackMsg, feedbackColor);
        }
        else {
            // Nuta istnieje, ale kliknięto za wcześnie lub za późno
            resetCombo();
            game.addFeedback(targetLane, "MISS", sf::Color::Red);
        }
    } else {
        // Brak nuty w tej ścieżce
        resetCombo();
        game.addFeedback(targetLane, "MISS", sf::Color::Red);
    }
}

//  Sprawdza, czy trzymano klawisz wystarczająco długo
void InputHandler::handleKeyRelease(char key, float currentTimeMs, Game& game) {
    int targetLane = -1;
    key = std::toupper(key);
    if (key == 'A') targetLane = 1; else if (key == 'D') targetLane = 2; else if (key == 'G') targetLane = 3; else if (key == 'J') targetLane = 4; else if (key == 'L') targetLane = 5;
    if (targetLane == -1) return;

    // Sprawdzamy czy na tym pasie trwała długa nuta
    if (laneHoldExpectedDuration[targetLane] > 0) {
        float heldTime = currentTimeMs - laneHoldStartTime[targetLane];
        if (heldTime >= laneHoldExpectedDuration[targetLane]) {
            totalScore += 20;
            game.addFeedback(targetLane, "+20 HOLD", sf::Color::Magenta);
        }
        laneHoldExpectedDuration[targetLane] = 0; // Reset po puszczeniu
    }
}

int InputHandler::getTotalScore() const { return totalScore; }
void InputHandler::resetCombo() { lastHit = HitAccuracy::MISS; }