#pragma once
#include <SFML/Graphics.hpp>

// Klasa bazowa dla wszystkich spadających klocków
class Note {
protected:
    int lane;
    int targetTime;
    int duration;

    float yPos;
    float speed;

    float rotation;
    float rotationSpeed;

public:
    Note(int lane, int targetTime, int duration, float speed, float rotationSpeed);
    virtual ~Note() = default;

    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual void update(float deltaTime);

    int getLane() const;
    int getTargetTime() const;
    int getDuration() const;
    float getY() const;
};
