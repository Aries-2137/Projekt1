#include "Note.h"

Note::Note(int lane, int targetTime, int duration, float speed, float rotationSpeed)
    : lane(lane), targetTime(targetTime), duration(duration), yPos(-100.0f),
    speed(speed), rotation(0.0f), rotationSpeed(rotationSpeed) {}

void Note::updateSynced(float currentSongPositionMs) {
    float timeDifference = static_cast<float>(targetTime) - currentSongPositionMs;
    yPos = 750.0f - (timeDifference * 0.5f);
    rotation = currentSongPositionMs * (rotationSpeed / 100.0f);
}

void Note::draw(sf::RenderWindow& window) const {
    sf::CircleShape circle(25.f);
    circle.setOrigin(25.f, 25.f);

    // Pozycjonowanie X zsynchronizowane z receptorami w Game::draw
    float xPos = 50.0f + (lane * 100.0f);
    circle.setPosition(xPos, yPos);

    if (lane == 1) circle.setFillColor(sf::Color::Green);
    else if (lane == 2) circle.setFillColor(sf::Color::Red);
    else if (lane == 3) circle.setFillColor(sf::Color::Yellow);
    else if (lane == 4) circle.setFillColor(sf::Color::Blue);
    else circle.setFillColor(sf::Color::Magenta);

    window.draw(circle);
}

int Note::getLane() const { return lane; }
int Note::getTargetTime() const { return targetTime; }
int Note::getDuration() const { return duration; }
float Note::getY() const { return yPos; }