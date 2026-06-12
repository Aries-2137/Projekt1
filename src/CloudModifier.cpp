#include "CloudModifier.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <algorithm>

CloudModifier::CloudModifier() :
    active(false), spawnTimer(0.0f), maxSpawnTime(2.5f),
    isDraggingSlider(false), sliderValue(0.5f)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // --- NOWE WSPÓŁRZĘDNE: PRAWY DOLNY RÓG ---
    // Bazowe pozycje dla wyrównania elementów
    float baseX = 520.f;
    float baseY = 460.f;

    // 1. Konfiguracja kwadratowego przycisku włącznika
    toggleButton.setSize(sf::Vector2f(30.f, 30.f));
    toggleButton.setPosition(baseX, baseY);
    toggleButton.setFillColor(sf::Color::Red);
    toggleButton.setOutlineThickness(2.f);
    toggleButton.setOutlineColor(sf::Color::White);

    if (font.loadFromFile("assets/arial.ttf")) {
        // 2. Tekst obok włącznika
        buttonText.setFont(font);
        buttonText.setString("Utrudnienie (Chmury)");
        buttonText.setCharacterSize(18); // Lekko zmniejszony font, by idealnie pasował do rogu
        buttonText.setFillColor(sf::Color::White);
        buttonText.setPosition(baseX + 40.f, baseY + 3.f);

        // 3. Tekst informacyjny nad suwakiem
        sliderText.setFont(font);
        sliderText.setString("Intensywnosc chmur:");
        sliderText.setCharacterSize(16);
        sliderText.setFillColor(sf::Color::White);
        sliderText.setPosition(baseX, baseY + 50.f);
    }

    // 4. Tor suwaka (szerokość dopasowana do prawego boku ekranu: 200px)
    sliderTrack.setSize(sf::Vector2f(200.f, 6.f));
    sliderTrack.setPosition(baseX, baseY + 85.f);
    sliderTrack.setFillColor(sf::Color(100, 100, 100));

    // 5. Uchwyt suwaka (kropka)
    sliderHandle.setRadius(10.f);
    sliderHandle.setOrigin(10.f, 10.f);

    // Obliczamy pozycję początkową kropki na bazie sliderValue (0.5f) na nowym torze
    float initialHandleX = baseX + (sliderTrack.getSize().x * sliderValue);
    sliderHandle.setPosition(initialHandleX, baseY + 88.f);
    sliderHandle.setFillColor(sf::Color::Cyan);
}

void CloudModifier::toggle() {
    active = !active;
    toggleButton.setFillColor(active ? sf::Color::Green : sf::Color::Red);
    if (!active) clouds.clear();
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
    // 1. Sprawdzenie kliknięcia w kwadrat
    sf::FloatRect buttonBounds = toggleButton.getGlobalBounds();
    if (buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
        toggle();
        return;
    }

    // 2. NOWOŚĆ: Sprawdzenie kliknięcia w uchwyt suwaka
    sf::FloatRect handleBounds = sliderHandle.getGlobalBounds();
    if (handleBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
        isDraggingSlider = true;
    }
}

// NOWOŚĆ: Puszczenie przycisku myszy przerywa przeciąganie
void CloudModifier::handleMouseRelease() {
    isDraggingSlider = false;
}

// NOWOŚĆ: Aktualizacja pozycji suwaka podczas ruchu myszką
void CloudModifier::handleMouseMove(const sf::Vector2i& mousePos) {
    if (!isDraggingSlider) return;

    float minX = sliderTrack.getPosition().x;
    float maxX = minX + sliderTrack.getSize().x;

    // Ograniczamy ruch uchwytu do granic toru suwaka
    float currentX = std::max(minX, std::min(static_cast<float>(mousePos.x), maxX));
    sliderHandle.setPosition(currentX, sliderHandle.getPosition().y);

    // Wyliczamy wartość od 0.0f do 1.0f
    sliderValue = (currentX - minX) / sliderTrack.getSize().x;
}

void CloudModifier::spawnCloud() {
    Cloud newCloud;
    newCloud.shape.setPointCount(6);
    newCloud.shape.setPoint(0, sf::Vector2f(0.f, 40.f));
    newCloud.shape.setPoint(1, sf::Vector2f(30.f, 15.f));
    newCloud.shape.setPoint(2, sf::Vector2f(70.f, 10.f));
    newCloud.shape.setPoint(3, sf::Vector2f(110.f, 25.f));
    newCloud.shape.setPoint(4, sf::Vector2f(140.f, 45.f));
    newCloud.shape.setPoint(5, sf::Vector2f(70.f, 60.f));

    newCloud.shape.setFillColor(sf::Color(240, 240, 245, 210));
    newCloud.xPos = -160.f;

    // --- POPRAWKA: ROZSZERZENIE ZAKRESU W DÓŁ ---
    // Zmieniamy z % 300 + 80 (zakres 80-380) na % 400 + 80 (zakres 80-480)
    // Chmury mogą teraz latać o 100 pikseli niżej niż do tej pory!
    newCloud.yPos = static_cast<float>(std::rand() % 400 + 80);

    // Obliczanie prędkości na podstawie suwaka (bez zmian)
    float speedMultiplier = 1.0f + (sliderValue * 1.5f);
    newCloud.speed = static_cast<float>(std::rand() % 80 + 60) * speedMultiplier;

    newCloud.scale = static_cast<float>(std::rand() % 10 + 10) / 10.0f;
    newCloud.shape.setScale(newCloud.scale, newCloud.scale);

    clouds.push_back(newCloud);
}

void CloudModifier::update(float deltaTime) {
    if (!active) return;

    // MODYFIKACJA: Suwak wpływa na częstotliwość spawnu chmur
    // Jeśli suwak = 0.0f, chmura pojawia się co 3.5 sekundy (łatwo)
    // Jeśli suwak = 1.0f, chmura pojawia się co 0.8 sekundy (trudno, gęsto)
    float currentMaxSpawnTime = 3.5f - (sliderValue * 2.7f);

    spawnTimer += deltaTime;
    if (spawnTimer >= currentMaxSpawnTime) {
        spawnTimer = 0.0f;
        spawnCloud();
    }

    for (auto it = clouds.begin(); it != clouds.end(); ) {
        it->xPos += it->speed * deltaTime;
        it->shape.setPosition(it->xPos, it->yPos);

        if (it->xPos > 850.f) {
            it = clouds.erase(it);
        } else {
            ++it;
        }
    }
}

void CloudModifier::draw(sf::RenderWindow& window) {
    if (active) {
        for (const auto& cloud : clouds) {
            window.draw(cloud.shape);
        }
    }
}

// Rysowanie pełnego panelu sterowania w menu głównym
void CloudModifier::drawButton(sf::RenderWindow& window) {
    window.draw(toggleButton);
    window.draw(buttonText);

    // Rysowanie suwaka
    window.draw(sliderText);
    window.draw(sliderTrack);
    window.draw(sliderHandle);
}