#include "Note.h"

Note::Note(int lane, int targetTime, int duration, float speed, float rotationSpeed)
    : lane(lane), targetTime(targetTime), duration(duration), yPos(-100.0f),
    speed(speed), rotation(0.0f), rotationSpeed(rotationSpeed) {}

void Note::updateSynced(float currentSongPositionMs) {
    float timeDifference = static_cast<float>(targetTime) - currentSongPositionMs;
    yPos = 750.0f - (timeDifference * 0.5f);
    rotation = currentSongPositionMs * (rotationSpeed / 100.0f);
}

// Implementacja rysowania pojedynczej nuty na danej ścieżce (lane)
void Note::draw(sf::RenderWindow& window) const {
    sf::CircleShape circle(30.f);
    circle.setOrigin(30.f, 30.f);

    // Rozmieszczenie nut w 5 kolumnach co 120 pikseli
    circle.setPosition(160.f + lane * 120.f, yPos);

    // Kolor zależny od kolumny (lane)
    if (lane == 0) circle.setFillColor(sf::Color::Red);
    else if (lane == 1) circle.setFillColor(sf::Color::Blue);
    else if (lane == 2) circle.setFillColor(sf::Color::Yellow);
    else if (lane == 3) circle.setFillColor(sf::Color::Green);
    else circle.setFillColor(sf::Color::Magenta);

    window.draw(circle);
}

int Note::getLane() const { return lane; }
int Note::getTargetTime() const { return targetTime; }
int Note::getDuration() const { return duration; }
float Note::getY() const { return yPos; }