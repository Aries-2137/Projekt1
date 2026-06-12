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
    float maxSpawnTime; // Podstawowy czas spawnu chmur

    // Elementy graficzne przycisku włącznika
    sf::RectangleShape toggleButton;
    sf::Font font;
    sf::Text buttonText;

    // Elementy graficzne suwaka (Slidera)
    sf::RectangleShape sliderTrack;  // Linia suwaka
    sf::CircleShape sliderHandle;    // Uchwyt/Kropka do przesuwania
    sf::Text sliderText;             // Napis informacyjny
    bool isDraggingSlider;           // Czy gracz aktualnie przeciąga suwak myszką
    float sliderValue;               // Wartość suwaka od 0.0f (łatwo) do 1.0f (trudno)

    void spawnCloud();

public:
    CloudModifier();
    ~CloudModifier() = default;

    void toggle();
    void setActive(bool state);
    bool isActive() const;

    // Interakcja za pomocą myszy (wywoływane w main.cpp)
    void handleMouseClick(const sf::Vector2i& mousePos);
    void handleMouseRelease();
    void handleMouseMove(const sf::Vector2i& mousePos);

    // Logika i rysowanie
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);       // Rysuje chmury podczas gry
    void drawButton(sf::RenderWindow& window); // Rysuje panel w menu głównym
};