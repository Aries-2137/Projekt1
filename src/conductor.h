#pragma once
#include <SFML/Audio.hpp>

class Conductor {
public:
    static float songPosition;     // Aktualna pozycja utworu w ms
    static float offset;           // Offset startowy w ms
    static float bpm;              // Tempo utworu (Beats Per Minute)
    static float beatDuration;     // Czas trwania jednego beatu w ms
    static int currentBeat;        // Licznik beatów w utworze (od 0)

    // NOWOŚĆ:
    static float beatProgress;     // Płynny postęp obecnego uderzenia (od 0.0 do 1.0)
    static bool justTriggeredBeat; // Flaga aktywująca się na jedną klatkę w momencie uderzenia

    static void initialize(float bpm, float offset);
    static void update(sf::Music& song);
};