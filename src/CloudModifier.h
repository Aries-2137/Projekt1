#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class CloudModifier {
private:
    struct Cloud {
        sf::ConvexShape shape;
        float xPos;
        float yPos;
        float speed;
        float scale;
    };

    bool active;
    std::vector<Cloud> clouds;
    float spawnTimer;
    float maxSpawnTime;

    // Elementy graficzne przycisku
    sf::RectangleShape toggleButton;
    sf::Font font;
    sf::Text buttonText;

    sf::RectangleShape sliderTrack;  // Linia suwaka
    sf::CircleShape sliderHandle;    // Uchwyt/Kropka do przesuwania
    sf::Text sliderText;             // Napis informacyjny
    bool isDraggingSlider;           // Czy gracz aktualnie trzyma suwak myszką
    float sliderValue;

    void spawnCloud();

public:
    CloudModifier();

    void toggle();
    void setActive(bool state);
    bool isActive() const;

    void handleMouseClick(const sf::Vector2i& mousePos);
    void handleMouseRelease();
    void handleMouseMove(const sf::Vector2i& mousePos); // NOWOŚĆ dla suwaka

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    void drawButton(sf::RenderWindow& window);
};
