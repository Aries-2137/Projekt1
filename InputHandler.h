#pragma once
#include <vector>
#include <memory>
#include "Note.h"

class Game; // Deklaracja zapowiadająca

enum class HitAccuracy { PERFECT, SUPER, GOOD, MISS, NONE };

class InputHandler {
private:
    int totalScore;
    HitAccuracy lastHit;

    // Tablice do śledzenia przytrzymania klawisza (dla 5 ścieżek, indeksy 1-5)
    float laneHoldStartTime[6] = {0.0f};
    int laneHoldExpectedDuration[6] = {0};

public:
    InputHandler();
    void handleKeyPress(char key, float currentTimeMs, std::vector<std::unique_ptr<Note>>& activeNotes, Game& game);

    void handleKeyRelease(char key, float currentTimeMs, Game& game);

    int getTotalScore() const;
    void resetCombo();
};