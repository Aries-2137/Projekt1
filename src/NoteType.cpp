#include "NoteType.h"
#include <SFML/Graphics.hpp>

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

void drawNoteWithTail(sf::RenderWindow& window, int lane, float yPos, float rotation, int duration) {
    float x = getXForLane(lane);
    sf::Color color = getColorForLane(lane);

    // Obliczamy fizyczną długość ogona, od razu ucinając 35 pikseli!
    // (25 px to ominięcie głowy następnej nuty + 10 px czystego marginesu)
    float tailLength = (static_cast<float>(duration) * 0.5f) - 35.0f;

    // Rysujemy ogon tylko, gdy nuta jest holdem i ogon ma sensowną długość
    if (duration > 1 && tailLength > 0.0f) {
        sf::RectangleShape tail(sf::Vector2f(15.0f, tailLength));

        // ZAKOTWICZENIE NA DOLE
        tail.setOrigin(7.5f, tailLength);
        tail.setPosition(x, yPos);

        sf::Color tailColor = color;
        tailColor.a = 150;
        tail.setFillColor(tailColor);

        window.draw(tail);
    }

    // Rysowanie głowy nuty
    sf::CircleShape circle(25.0f);
    circle.setOrigin(25.0f, 25.0f);
    circle.setPosition(x, yPos);
    circle.setRotation(rotation);
    circle.setFillColor(color);

    window.draw(circle);
}

NoteType1::NoteType1(int lane, int targetTime, float speed, float rotationSpeed)
    : Note(lane, targetTime, 1, speed, rotationSpeed) {}

// Typ 2: Dopasowany do odstępów wynoszących ok. 625-630 ms (np. 35537 - 34911)
NoteType2::NoteType2(int lane, int targetTime, float speed, float rotationSpeed)
    : Note(lane, targetTime, 625, speed, rotationSpeed) {}

// Typ 3 i 4: Obecnie nieużywane w mapie, ale zbalansowane proporcjonalnie
NoteType3::NoteType3(int lane, int targetTime, float speed, float rotationSpeed)
    : Note(lane, targetTime, 1250, speed, rotationSpeed) {}

NoteType4::NoteType4(int lane, int targetTime, float speed, float rotationSpeed)
    : Note(lane, targetTime, 1875, speed, rotationSpeed) {}

// Typ 5: Idealnie zsynchronizowany z odstępem 2535 ms z początku mapy
NoteType5::NoteType5(int lane, int targetTime, float speed, float rotationSpeed)
    : Note(lane, targetTime, 2535, speed, rotationSpeed) {}

void NoteType1::draw(sf::RenderWindow& window) const { drawNoteWithTail(window, lane, yPos, rotation, duration); }
void NoteType2::draw(sf::RenderWindow& window) const { drawNoteWithTail(window, lane, yPos, rotation, duration); }
void NoteType3::draw(sf::RenderWindow& window) const { drawNoteWithTail(window, lane, yPos, rotation, duration); }
void NoteType4::draw(sf::RenderWindow& window) const { drawNoteWithTail(window, lane, yPos, rotation, duration); }
void NoteType5::draw(sf::RenderWindow& window) const { drawNoteWithTail(window, lane, yPos, rotation, duration); }