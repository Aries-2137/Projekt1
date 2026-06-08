#include "NoteType.h"
#include <SFML/Graphics.hpp>

// ustalanie  koloru nuty na podstawie ścieżki (lane 1-5)
sf::Color getColorForLane(int lane) {
    switch (lane) {
    case 1: return sf::Color::Green;
    case 2: return sf::Color::Red;
    case 3: return sf::Color::Yellow;
    case 4: return sf::Color::Blue;
    case 5: return sf::Color::Magenta;
    default: return sf::Color::White;
    }
}

float getXForLane(int lane) {
    return 50.0f + (lane * 100.0f);
}

NoteType1::NoteType1(int lane, int targetTime, float speed, float rotationSpeed)
    : Note(lane, targetTime, 1, speed, rotationSpeed) {}

void NoteType1::draw(sf::RenderWindow& window) const {
    sf::CircleShape circle(25.0f); // Promień 25px
    circle.setOrigin(25.0f, 25.0f);
    circle.setPosition(getXForLane(lane), yPos);
    circle.setRotation(rotation);
    circle.setFillColor(getColorForLane(lane));

    window.draw(circle);
}

NoteType2::NoteType2(int lane, int targetTime, float speed, float rotationSpeed)
    : Note(lane, targetTime, 2, speed, rotationSpeed) {}

void NoteType2::draw(sf::RenderWindow& window) const {
    float x = getXForLane(lane);
    float tailLength = 50.0f; // długość ogona

    // Rysujemy ogon
    sf::RectangleShape tail(sf::Vector2f(15.0f, tailLength));
    tail.setOrigin(7.5f, tailLength);
    tail.setPosition(x, yPos);
    tail.setFillColor(getColorForLane(lane));

    sf::CircleShape circle(25.0f);
    circle.setOrigin(25.0f, 25.0f);
    circle.setPosition(x, yPos);
    circle.setRotation(rotation);
    circle.setFillColor(getColorForLane(lane));

    // Rysujemy w kolejności: najpierw ogon (pod spodem), potem koło (na wierzchu)
    window.draw(tail);
    window.draw(circle);
}

NoteType3::NoteType3(int lane, int targetTime, float speed, float rotationSpeed)
    : Note(lane, targetTime, 3, speed, rotationSpeed) {}

void NoteType3::draw(sf::RenderWindow& window) const {
    float x = getXForLane(lane);
    float tailLength = 100.0f;

    sf::RectangleShape tail(sf::Vector2f(15.0f, tailLength));
    tail.setOrigin(7.5f, tailLength);
    tail.setPosition(x, yPos);
    tail.setFillColor(getColorForLane(lane));

    sf::CircleShape circle(25.0f);
    circle.setOrigin(25.0f, 25.0f);
    circle.setPosition(x, yPos);
    circle.setRotation(rotation);
    circle.setFillColor(getColorForLane(lane));

    window.draw(tail);
    window.draw(circle);
}

NoteType4::NoteType4(int lane, int targetTime, float speed, float rotationSpeed)
    : Note(lane, targetTime, 4, speed, rotationSpeed) {}

void NoteType4::draw(sf::RenderWindow& window) const {
    float x = getXForLane(lane);
    float tailLength = 150.0f;

    sf::RectangleShape tail(sf::Vector2f(15.0f, tailLength));
    tail.setOrigin(7.5f, tailLength);
    tail.setPosition(x, yPos);
    tail.setFillColor(getColorForLane(lane));

    sf::CircleShape circle(25.0f);
    circle.setOrigin(25.0f, 25.0f);
    circle.setPosition(x, yPos);
    circle.setRotation(rotation);
    circle.setFillColor(getColorForLane(lane));

    window.draw(tail);
    window.draw(circle);
}

NoteType5::NoteType5(int lane, int targetTime, float speed, float rotationSpeed)
    : Note(lane, targetTime, 5, speed, rotationSpeed) {}

void NoteType5::draw(sf::RenderWindow& window) const {
    float x = getXForLane(lane);
    float tailLength = 200.0f;

    sf::RectangleShape tail(sf::Vector2f(15.0f, tailLength));
    tail.setOrigin(7.5f, tailLength);
    tail.setPosition(x, yPos);
    tail.setFillColor(getColorForLane(lane));

    sf::CircleShape circle(25.0f);
    circle.setOrigin(25.0f, 25.0f);
    circle.setPosition(x, yPos);
    circle.setRotation(rotation);
    circle.setFillColor(getColorForLane(lane));

    window.draw(tail);
    window.draw(circle);
}