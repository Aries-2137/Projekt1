#pragma once
#include <SFML/Graphics.hpp>

class Note {
protected:
    int lane;
    int targetTime;
    int duration;
    float yPos; // To pole musi tu być!
    float speed;
    float rotation;
    float rotationSpeed;

public:
    Note(int lane, int targetTime, int duration, float speed, float rotationSpeed);
    virtual ~Note() = default;

    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual void updateSynced(float currentSongPositionMs);

    int getLane() const;
    int getTargetTime() const;
    int getDuration() const;
    float getY() const;
};