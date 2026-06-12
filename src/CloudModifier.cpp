#include "CloudModifier.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>

CloudModifier::CloudModifier() :
    active(false), spawnTimer(0.0f), maxSpawnTime(2.5f),
    isDraggingSlider(false), sliderValue(0.5f) // Domyślnie suwak na środku
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Punkty odniesienia dla prawego dolnego rogu (lekko przesunięte w dół)
    float baseX = 520.f;
    float baseY = 485.f;

    // 1. Konfiguracja kwadratowego przycisku włącznika
    toggleButton.setSize(sf::Vector2f(30.f, 30.f));
    toggleButton.setPosition(baseX, baseY);
    toggleButton.setFillColor(sf::Color::Red); // Domyślnie wyłączone (czerwony)
    toggleButton.setOutlineThickness(2.f);
    toggleButton.setOutlineColor(sf::Color::White);

    if (font.loadFromFile("assets/arial.ttf")) {
        // 2. Tekst obok włącznika
        buttonText.setFont(font);
        buttonText.setString("Utrudnienie (Chmury)");
        buttonText.setCharacterSize(18);
        buttonText.setFillColor(sf::Color::White);
        buttonText.setPosition(baseX + 40.f, baseY + 3.f);

        // 3. Tekst informacyjny nad suwakiem
        sliderText.setFont(font);
        sliderText.setString("Intensywnosc chmur:");
        sliderText.setCharacterSize(16);
        sliderText.setFillColor(sf::Color::White);
        sliderText.setPosition(baseX, baseY + 45.f);
    }

    // 4. Tor suwaka (szerokość 200px)
    sliderTrack.setSize(sf::Vector2f(200.f, 6.f));
    sliderTrack.setPosition(baseX, baseY + 75.f);
    sliderTrack.setFillColor(sf::Color(100, 100, 100));

    // 5. Uchwyt suwaka (cyjanowa kropka)
    sliderHandle.setRadius(10.f);
    sliderHandle.setOrigin(10.f, 10.f);

    // Obliczanie początkowej pozycji na osi X dla wartości 0.5f
    float initialHandleX = baseX + (sliderTrack.getSize().x * sliderValue);
    sliderHandle.setPosition(initialHandleX, baseY + 78.f);
    sliderHandle.setFillColor(sf::Color::Cyan);
}

void CloudModifier::toggle() {
    active = !active;
    toggleButton.setFillColor(active ? sf::Color::Green : sf::Color::Red);
    if (!active) clouds.clear(); // Czyści chmury natychmiast po wyłączeniu
}

void CloudModifier::setActive(bool state) {
    active = state;
    toggleButton.setFillColor(active ? sf::Color::Green : sf::Color::Red);
    if (!active) clouds.clear();
}

bool CloudModifier::isActive() const {
    return active;
}

void CloudModifier::handleMouseClick(const sf::Vector2i& mousePos) {
    // Sprawdzenie kliknięcia we włącznik (kwadrat)
    sf::FloatRect buttonBounds = toggleButton.getGlobalBounds();
    if (buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
        toggle();
        return;
    }

    // Sprawdzenie kliknięcia w kropkę suwaka
    sf::FloatRect handleBounds = sliderHandle.getGlobalBounds();
    if (handleBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
        isDraggingSlider = true;
    }
}

void CloudModifier::handleMouseRelease() {
    isDraggingSlider = false;
}

void CloudModifier::handleMouseMove(const sf::Vector2i& mousePos) {
    if (!isDraggingSlider) return;

    float minX = sliderTrack.getPosition().x;
    float maxX = minX + sliderTrack.getSize().x;

    // Ograniczenie ruchu kropki do szerokości paska suwaka
    float currentX = std::max(minX, std::min(static_cast<float>(mousePos.x), maxX));
    sliderHandle.setPosition(currentX, sliderHandle.getPosition().y);

    // Przeliczenie pozycji X na wartość z przedziału od 0.0f do 1.0f
    sliderValue = (currentX - minX) / sliderTrack.getSize().x;
}

void CloudModifier::spawnCloud() {
    Cloud newCloud;

    // Proceduralne rysowanie chmurki z 6 wierzchołków (nie wymaga tekstur .png)
    newCloud.shape.setPointCount(6);
    newCloud.shape.setPoint(0, sf::Vector2f(0.f, 40.f));
    newCloud.shape.setPoint(1, sf::Vector2f(30.f, 15.f));
    newCloud.shape.setPoint(2, sf::Vector2f(70.f, 10.f));
    newCloud.shape.setPoint(3, sf::Vector2f(110.f, 25.f));
    newCloud.shape.setPoint(4, sf::Vector2f(140.f, 45.f));
    newCloud.shape.setPoint(5, sf::Vector2f(70.f, 60.f));

    // Lekko półprzezroczysty biało-szary kolor
    newCloud.shape.setFillColor(sf::Color(240, 240, 245, 210));
    newCloud.xPos = -160.f; // Spawnuje się tuż za lewą krawędzią

    // ROZSZERZONY ZAKRES W DÓŁ: Chmury latają w przedziale Y: 80 do 480 (środek ekranu)
    newCloud.yPos = static_cast<float>(std::rand() % 400 + 80);

    // Prędkość modyfikowana suwakiem (od 1.0x do 2.5x bazowej prędkości)
    float speedMultiplier = 1.0f + (sliderValue * 1.5f);
    newCloud.speed = static_cast<float>(std::rand() % 80 + 60) * speedMultiplier;

    // Losowa skala wielkości chmury
    newCloud.scale = static_cast<float>(std::rand() % 10 + 10) / 10.0f;
    newCloud.shape.setScale(newCloud.scale, newCloud.scale);

    clouds.push_back(newCloud);
}

void CloudModifier::update(float deltaTime) {
    if (!active) return;

    // Częstotliwość spawnu modyfikowana suwakiem:
    // sliderValue = 0.0f -> spawn co 3.5s (rzadko)
    // sliderValue = 1.0f -> spawn co 0.8s (bardzo gęsto)
    float currentMaxSpawnTime = 3.5f - (sliderValue * 2.7f);

    spawnTimer += deltaTime;
    if (spawnTimer >= currentMaxSpawnTime) {
        spawnTimer = 0.0f;
        spawnCloud();
    }

    // Ruch chmur w prawo
    for (auto it = clouds.begin(); it != clouds.end(); ) {
        it->xPos += it->speed * deltaTime;
        it->shape.setPosition(it->xPos, it->yPos);

        // Usuwanie, kiedy chmura wyleci całkowicie za prawą krawędź (szerokość okna = 800)
        if (it->xPos > 850.f) {
            it = clouds.erase(it);
        } else {
            ++it;
        }
    }
}

// Rysowanie samych chmur podczas rozgrywki (PLAYING)
void CloudModifier::draw(sf::RenderWindow& window) {
    if (active) {
        for (const auto& cloud : clouds) {
            window.draw(cloud.shape);
        }
    }
}

// Rysowanie całego interaktywnego panelu kontrolnego w menu głównym (MENU)
void CloudModifier::drawButton(sf::RenderWindow& window) {
    window.draw(toggleButton);
    window.draw(buttonText);
    window.draw(sliderText);
    window.draw(sliderTrack);
    window.draw(sliderHandle);
}