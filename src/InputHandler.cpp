#include "InputHandler.h"
#include "Game.h"
#include "Conductor.h" // DODANO
#include <iostream>
#include <cmath>
#include <cctype>

InputHandler::InputHandler() : totalScore(0), lastHit(HitAccuracy::NONE) {}

void InputHandler::handleKeyPress(char key, float currentTimeMs, std::vector<std::unique_ptr<Note>>& activeNotes, Game& game) {
    // NOWOŚĆ: Synchronizacja systemu wejściowego z osia czasu Conductora (likwiduje problem fałszywych MISS)
    currentTimeMs = Conductor::songPosition;

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

        // Nuta musi znajdować się w strefie kliknięcia (Y > 600)
        if (diff <= 150.0f) { // Sprawdzamy tylko różnicę czasu (muzyka vs targetTime)
            game.triggerReceptorAnimation(targetLane);
            int duration = (*closestNoteIt)->getDuration();
            activeNotes.erase(closestNoteIt);

            std::string feedbackMsg;
            sf::Color feedbackColor;

            if (diff <= 30.0f) {
                totalScore += (lastHit == HitAccuracy::PERFECT) ? 100 : 50;
                lastHit = HitAccuracy::PERFECT;
                feedbackMsg = "PERFECT!";
                feedbackColor = sf::Color::Cyan;
            }
            else if (diff <= 60.0f) {
                totalScore += (lastHit == HitAccuracy::PERFECT || lastHit == HitAccuracy::SUPER) ? 50 : 20;
                lastHit = HitAccuracy::SUPER;
                feedbackMsg = "SUPER!";
                feedbackColor = sf::Color::Green;
            }
            else {
                totalScore += 10;
                lastHit = HitAccuracy::GOOD;
                feedbackMsg = "GOOD";
                feedbackColor = sf::Color::Yellow;
            }

            if (duration > 1) {
                laneHoldStartTime[targetLane] = currentTimeMs;
                laneHoldExpectedDuration[targetLane] = (duration - 1) * 250;
            }
            game.addFeedback(targetLane, feedbackMsg, feedbackColor);
        } else {
            resetCombo();
            game.addFeedback(targetLane, "MISS", sf::Color::Red);
        }
    } else {
        resetCombo();
        game.addFeedback(targetLane, "MISS", sf::Color::Red);
    }
}

void InputHandler::handleKeyRelease(char key, float currentTimeMs, Game& game) {
    // Logika pusta dla kompatybilności linkera
}

int InputHandler::getTotalScore() const { return totalScore; }
void InputHandler::resetCombo() { lastHit = HitAccuracy::NONE; }