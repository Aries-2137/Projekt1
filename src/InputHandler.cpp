#include "InputHandler.h"
#include "Game.h"
#include "Conductor.h"
#include <iostream>
#include <cmath>
#include <cctype>

InputHandler::InputHandler() : totalScore(0), lastHit(HitAccuracy::NONE) {}

void InputHandler::handleKeyPress(char key, float currentTimeMs, std::vector<std::unique_ptr<Note>>& activeNotes, Game& game) {
    currentTimeMs = Conductor::songPosition;

    int targetLane = -1;
    key = std::toupper(key);

    if (key == 'P') {
        game.togglePause();
        return;
    }

    if (game.getIsPaused()) return;

    if (key == 'S') targetLane = 1;
    else if (key == 'D') targetLane = 2;
    else if (key == ' ') targetLane = 3;
    else if (key == 'J') targetLane = 4;
    else if (key == 'K') targetLane = 5;

    if (targetLane == -1) return;

    auto closestNoteIt = activeNotes.end();

    float minDiff = 200.0f;

    for (auto it = activeNotes.begin(); it != activeNotes.end(); ++it) {
        if ((*it)->getLane() == targetLane) {
            float noteTargetTime = static_cast<float>((*it)->getTargetTime());
            float diff = std::abs(noteTargetTime - currentTimeMs);

            if (diff < minDiff) {
                minDiff = diff;
                closestNoteIt = it;
            }
        }
    }

    if (closestNoteIt != activeNotes.end()) {
        float diff = minDiff;
        std::string feedbackMsg = "MISS";
        sf::Color feedbackColor = sf::Color::Red;

        if (diff <= 45.0f) {
            totalScore += (lastHit == HitAccuracy::PERFECT) ? 100 : 50;
            lastHit = HitAccuracy::PERFECT;
            feedbackMsg = "PERFECT!";
            feedbackColor = sf::Color::Cyan;
        }
        else if (diff <= 90.0f) {
            totalScore += (lastHit == HitAccuracy::PERFECT || lastHit == HitAccuracy::SUPER) ? 50 : 20;
            lastHit = HitAccuracy::SUPER;
            feedbackMsg = "SUPER!";
            feedbackColor = sf::Color::Green;
        }
        else if (diff <= 180.0f) {
            totalScore += 10;
            lastHit = HitAccuracy::GOOD;
            feedbackMsg = "GOOD";
            feedbackColor = sf::Color::Yellow;
        }
        else {
            resetCombo();
            lastHit = HitAccuracy::MISS;
            feedbackMsg = "MISS";
            feedbackColor = sf::Color::Red;
        }

        if (lastHit != HitAccuracy::MISS) {
            if ((*closestNoteIt)->getDuration() > 1) {
                (*closestNoteIt)->setBeingHeld(true);
                (*closestNoteIt)->setLastScoredBeat(static_cast<int>(Conductor::currentBeat));
            } else {
                activeNotes.erase(closestNoteIt);
            }

            game.triggerReceptorAnimation(targetLane);
            game.addFeedback(targetLane, feedbackMsg, feedbackColor);
            return;
        }
    } else {
        resetCombo();
        game.addFeedback(targetLane, "MISS", sf::Color::Red);
    }
}

void InputHandler::handleKeyRelease(char key, float songPosition, Game& game) {

    std::cout << "[DEBUG INPUT]: Puszczono klawisz " << key << " - logika gry ignoruje to puszczenie.\n";
}

int InputHandler::getTotalScore() const {
    return totalScore;
}

void InputHandler::resetCombo() {
    lastHit = HitAccuracy::MISS;
}