#include "Interface.h"
#include <iostream>
#include <filesystem>

Interface::Interface() {
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Nie udalo sie zaladowac czcionki arial.ttf!" << std::endl;
    }

    // Automatyczne ładowanie max 10 utworów z folderu music_sequence
    songList.clear();
    std::string path = "music_sequence";

    if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                songList.push_back(entry.path().filename().string());
                if (songList.size() >= 10) break; // Zatrzymujemy się na 10 utworach
            }
        }
    }
}

void Interface::drawMenu(sf::RenderWindow& window) {
    std::string labels[] = {"Utwory", "Wyniki", "Wyjdz"};
    for(int i = 0; i < 3; ++i) {
        sf::RectangleShape rect(sf::Vector2f(200, 50));
        rect.setPosition(300, 200 + i * 100);
        rect.setFillColor(sf::Color(50, 50, 50));
        window.draw(rect);

        sf::Text text(labels[i], font, 20);
        text.setPosition(370, 210 + i * 100);
        text.setFillColor(sf::Color::White);
        window.draw(text);
    }
}

void Interface::drawSongSelect(sf::RenderWindow& window) {
    float startX = 120.0f;
    float startY = 250.0f;
    float rectWidth = 240.0f;
    float rectHeight = 60.0f;
    float gapX = 80.0f;
    float gapY = 40.0f;

    for(size_t i = 0; i < songList.size() && i < 10; ++i) {
        int col = i / 5;
        int row = i % 5;

        float xPos = startX + col * (rectWidth + gapX);
        float yPos = startY + row * (rectHeight + gapY);

        sf::RectangleShape rect(sf::Vector2f(rectWidth, rectHeight));
        rect.setPosition(xPos, yPos);
        rect.setFillColor(sf::Color(70, 70, 70));
        rect.setOutlineThickness(2.0f);
        rect.setOutlineColor(sf::Color(100, 100, 100));
        window.draw(rect);

        std::string displayName = songList[i];
        size_t lastDot = displayName.find_last_of(".");
        if (lastDot != std::string::npos) {
            displayName = displayName.substr(0, lastDot);
        }

        sf::Text text(displayName, font, 18);
        text.setPosition(xPos + 15.0f, yPos + 18.0f);
        text.setFillColor(sf::Color::White);
        window.draw(text);
    }
}
GameState Interface::handleMenuClick(sf::Vector2i mousePos) {
    if (mousePos.x >= 300 && mousePos.x <= 500) {
        if (mousePos.y >= 200 && mousePos.y <= 250) return GameState::SONG_SELECT;
        if (mousePos.y >= 400 && mousePos.y <= 450) exit(0);
    }
    return GameState::MENU;
}

std::string Interface::handleSongSelectClick(sf::Vector2i mousePos) {
    if (mousePos.x >= 300 && mousePos.x <= 1500) {
        if (mousePos.y >= 200 && mousePos.y <= 250) return songList[0];
        if (mousePos.y >= 300 && mousePos.y <= 350) return songList[1];
    }
    return "";
}