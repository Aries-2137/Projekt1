#include "Conductor.h"

float Conductor::songPosition = 0.0f;
float Conductor::offset = 0.0f;
float Conductor::bpm = 90.0f;
float Conductor::beatDuration = 500.0f;
int Conductor::currentBeat = 0;
float Conductor::beatProgress = 0.0f;
bool Conductor::justTriggeredBeat = false;

void Conductor::initialize(float newBpm, float newOffset) {
    bpm = newBpm;
    offset = newOffset;
    songPosition = 0.0f;
    currentBeat = 0;
    beatProgress = 0.0f;
    justTriggeredBeat = false;

    if (bpm > 0.0f) {
        beatDuration = 60000.0f / bpm;
    } else {
        beatDuration = 500.0f;
    }
}

void Conductor::update(sf::Music& song) {
    if (song.getStatus() == sf::SoundStream::Playing) {
        songPosition = song.getPlayingOffset().asMilliseconds() - offset;
        if (songPosition < 0.0f) songPosition = 0.0f;

        int lastBeat = currentBeat;
        currentBeat = static_cast<int>(songPosition / beatDuration);

        // Wykrywanie dokładnego momentu przejścia w nowy beat
        if (currentBeat > lastBeat) {
            justTriggeredBeat = true;
        } else {
            justTriggeredBeat = false;
        }

        // Obliczanie fazy beatu (0.0 przy uderzeniu -> dąży do 1.0 tuż przed kolejnym)
        float timeInCurrentBeat = songPosition - (currentBeat * beatDuration);
        beatProgress = timeInCurrentBeat / beatDuration;
    } else {
        justTriggeredBeat = false;
        beatProgress = 0.0f;
    }
}