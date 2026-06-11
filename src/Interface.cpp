#include "Interface.h"
#include <iostream>

Interface::Interface() {
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "[BLAD INTERFEJSU]: Nie znaleziono czcionki arial.ttf!\n";
    }

    // --- INICJALIZACJA MENU GŁÓWNEGO ---
    menuTitle.setFont(font);
    menuTitle.setString("AGH RHYTHM GAME");
    menuTitle.setCharacterSize(45);
    menuTitle.setFillColor(sf::Color::White);
    menuTitle.setPosition(200.f, 200.f);

    playButton.setFont(font);
    playButton.setString("1. GRAJ");
    playButton.setCharacterSize(35);
    playButton.setFillColor(sf::Color::Green);
    playButton.setPosition(320.f, 450.f);

    exitButton.setFont(font);
    exitButton.setString("2. WYJDZ");
    exitButton.setCharacterSize(35);
    exitButton.setFillColor(sf::Color::Red);
    exitButton.setPosition(320.f, 580.f);

    // --- INICJALIZACJA WYBORU UTWORÓW ---
    selectTitle.setFont(font);
    selectTitle.setString("WYBIERZ UTWOR:");
    selectTitle.setCharacterSize(40);
    selectTitle.setFillColor(sf::Color::Yellow);
    selectTitle.setPosition(220.f, 100.f);

    // Dokładne nazwy plików w Waszym folderze music_sequence
    songFiles = {
        "Lady Pank.txt",
        "muzyka_test.txt",
        "muzyka1.txt",
        "muzyka2.txt",
        "Utwór 2 level 1.txt",
        "Utwór 2 level 2.txt"
    };

    // Automatyczne układanie przycisków na ekranie jeden pod drugim
    float startY = 250.f;
    for (size_t i = 0; i < songFiles.size(); ++i) {
        sf::Text songText;
        songText.setFont(font);

        // Wyświetlamy ładną nazwę (usuwamy końcówkę .txt na ekranie)
        std::string displayName = songFiles[i].substr(0, songFiles[i].find_last_of("."));
        songText.setString(displayName);

        songText.setCharacterSize(28);
        songText.setFillColor(sf::Color::White);

        // Wyśrodkowanie pozycji napisów na osi X
        songText.setPosition(200.f, startY + (i * 70.f));

        songButtons.push_back(songText);
    }
}

void Interface::drawMenu(sf::RenderWindow& window) {
    window.draw(menuTitle);
    window.draw(playButton);
    window.draw(exitButton);
}

void Interface::drawSongSelect(sf::RenderWindow& window) {
    window.draw(selectTitle);
    for (const auto& button : songButtons) {
        window.draw(button);
    }
}

GameState Interface::handleMenuClick(const sf::Vector2i& mousePos) {
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // Dynamiczne sprawdzenie czy kliknięto w obszar tekstu "GRAJ"
    if (playButton.getGlobalBounds().contains(mousePosF)) {
        std::cout << "[INTERFEJS]: Kliknieto przycisk GRAJ. Przejscie do wyboru piosenek.\n";
        return GameState::SONG_SELECT;
    }

    // Sprawdzenie czy kliknięto w "WYJDZ"
    if (exitButton.getGlobalBounds().contains(mousePosF)) {
        std::cout << "[INTERFEJS]: Kliknieto WYJDZ.\n";
        exit(0);
    }

    return GameState::MENU;
}

std::string Interface::handleSongSelectClick(const sf::Vector2i& mousePos) {
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // Pętla sprawdza każdy wygenerowany przycisk piosenki
    for (size_t i = 0; i < songButtons.size(); ++i) {
        // Pobieramy rzeczywiste granice tekstu na ekranie (pudełko kolizji)
        sf::FloatRect bounds = songButtons[i].getGlobalBounds();

        // Celowo powiększamy margines klikalności o 15 pikseli wokół napisu, żeby łatwiej było trafić myszką
        bounds.left -= 15.f;
        bounds.width += 30.f;
        bounds.top -= 10.f;
        bounds.height += 20.f;

        if (bounds.contains(mousePosF)) {
            std::cout << "[SUKCES INTERFEJSU]: Trafiono w przycisk utworu: " << songFiles[i] << "\n";
            return songFiles[i]; // Zwraca pełną nazwę pliku z rozszerzeniem .txt
        }
    }

    std::cout << "[INTERFEJS]: Klikniecie poza przyciskami utworow (puste tlo).\n";
    return "";
}