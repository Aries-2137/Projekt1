#include "InputHandler.h"
#include "Game.h"
#include "Conductor.h"
#include <iostream>
#include <cmath>
#include <cctype>

InputHandler::InputHandler() : totalScore(0), lastHit(HitAccuracy::NONE) {}

void InputHandler::handleKeyPress(char key, float currentTimeMs, std::vector<std::unique_ptr<Note>>& activeNotes, Game& game) {
    // Pobranie czasu bezpośrednio z zegara audio
    currentTimeMs = Conductor::songPosition;

    // Jeśli Conductor podaje czas w sekundach (np. 12.94f), a nuty w mapie mają milisekundy (np. 12945)
    // to mnożymy czas przez 1000, aby skale się zrównały:
    if (currentTimeMs < 1000.0f) {
        currentTimeMs *= 1000.0f;
    }
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

    // MARGINES BŁĘDU (Okno czasowe):
    // Zwiększamy je do 200ms, aby gra była przyjemna i wybaczała drobne przesunięcia rytmiczne.
    float minDiff = 200.0f;

    // Szukamy najbliższej nuty na klikniętej ścieżce
    for (auto it = activeNotes.begin(); it != activeNotes.end(); ++it) {
        if ((*it)->getLane() == targetLane) {
            float noteTargetTime = static_cast<float>((*it)->getTargetTime());
            float diff = std::abs(noteTargetTime - currentTimeMs);

            if (diff < minDiff) {
                minDiff = diff;
                closestNoteIt = it; // Zapisujemy tę nutę jako cel
            }
        }
    }

    // Jeśli znaleźliśmy nutę w granicach naszego marginesu błędu (minDiff < 200ms)
    if (closestNoteIt != activeNotes.end()) {
        float diff = minDiff; // Dokładna różnica czasu kliknięcia od ideału
        int duration = (*closestNoteIt)->getDuration();

        std::string feedbackMsg = "MISS";
        sf::Color feedbackColor = sf::Color::Red;

        // NASTAWIANIE PROGÓW MARGINESU BŁĘDU:
        // Idealne trafienie (Perfect): różnica do 45ms
        if (diff <= 45.0f) {
            totalScore += (lastHit == HitAccuracy::PERFECT) ? 100 : 50;
            lastHit = HitAccuracy::PERFECT;
            feedbackMsg = "PERFECT!";
            feedbackColor = sf::Color::Cyan;
        }
        // Dobre trafienie (Super): różnica od 46ms do 90ms
        else if (diff <= 90.0f) {
            totalScore += (lastHit == HitAccuracy::PERFECT || lastHit == HitAccuracy::SUPER) ? 50 : 20;
            lastHit = HitAccuracy::SUPER;
            feedbackMsg = "SUPER!";
            feedbackColor = sf::Color::Green;
        }
        // Ledwo trafione (Good): różnica od 91ms do 180ms (szeroki margines błędu)
        else if (diff <= 180.0f) {
            totalScore += 10;
            lastHit = HitAccuracy::GOOD;
            feedbackMsg = "GOOD";
            feedbackColor = sf::Color::Yellow;
        }
        // Wszystko powyżej 180ms do 200ms traktowane jest jako spóźniony/za szybki klik
        else {
            resetCombo();
            lastHit = HitAccuracy::MISS;
            feedbackMsg = "MISS";
            feedbackColor = sf::Color::Red;
        }

        // Jeśli kliknięcie zostało uznane za udane (nie MISS)
        if (lastHit != HitAccuracy::MISS) {
            if (duration > 1) {
                laneHoldStartTime[targetLane] = currentTimeMs;
                laneHoldExpectedDuration[targetLane] = (duration - 1) * 250;
            }

            // Wizualne potwierdzenie trafienia w interface
            game.triggerReceptorAnimation(targetLane);
            game.addFeedback(targetLane, feedbackMsg, feedbackColor);

            // KLUCZOWE: Usuwamy nutę z gry, żeby nie można było jej kliknąć drugi raz!
            activeNotes.erase(closestNoteIt);
        } else {
            // Trafienie zakwalifikowane jako MISS z powodu zbyt dużej odchyłki
            game.addFeedback(targetLane, "MISS", sf::Color::Red);
        }

    } else {
        // Gracz kliknął klawisz, ale na tej ścieżce nie ma żadnej nuty w promieniu 200ms
        resetCombo();
        game.addFeedback(targetLane, "MISS", sf::Color::Red);
    }
}
void InputHandler::handleKeyRelease(char key, float currentTimeMs, Game& game) {
    // Logika pusta dla kompatybilności z pętlą główną main
    (void)key;
    (void)currentTimeMs;
    (void)game;
}

int InputHandler::getTotalScore() const {
    return totalScore;
}

void InputHandler::resetCombo() {
    lastHit = HitAccuracy::MISS;
}