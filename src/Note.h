#pragma once
#include <SFML/Graphics.hpp>

class Note {
protected:
    int lane;
    int targetTime;
    int duration;
    float yPos;
    float speed;
    float rotation;
    float rotationSpeed;

    bool isBeingHeld = false;
    int lastScoredBeat = -1;

public:
    Note(int lane, int targetTime, int duration, float speed, float rotationSpeed);
    virtual ~Note() = default;

    virtual void draw(sf::RenderWindow& window) const;
    virtual void updateSynced(float currentSongPositionMs);

    int getLane() const;
    int getTargetTime() const;
    int getDuration() const;
    float getY() const;
    float releaseTimer = 0.0f;

    // Jeśli duration z pliku mapy > 1, to obiekt automatycznie jest nutą długą
    virtual bool isHoldNote() const { return duration > 1; }

    void setBeingHeld(bool state) { isBeingHeld = state; }
    bool getIsBeingHeld() const { return isBeingHeld; }

    int getLastScoredBeat() const { return lastScoredBeat; }
    void setLastScoredBeat(int beat) { lastScoredBeat = beat; }

    // Obliczanie końca nuty na podstawie zapisanego czasu duration
    virtual bool isFinished(float currentSongPositionMs) const {
        if (isHoldNote()) {
            return currentSongPositionMs > (static_cast<float>(targetTime) + static_cast<float>(duration));
        }
        return currentSongPositionMs > static_cast<float>(targetTime);
    }
};