#include "Interface.h"
#include <iostream>

#include "Interface.h"
#include <iostream>

Interface::Interface() {
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cerr << "[BLAD INTERFEJSU]: Nie znaleziono czcionki arial.ttf!\n";
    }

    // --- INICJALIZACJA MENU GŁÓWNEGO ---
    menuTitle.setFont(font);
    menuTitle.setString("AGH RHYTHM GAME");
    menuTitle.setCharacterSize(45);
    menuTitle.setFillColor(sf::Color::White);
    menuTitle.setPosition(220.f, 150.f);

    playButton.setFont(font);
    playButton.setString("1. GRAJ");
    playButton.setCharacterSize(35);
    playButton.setFillColor(sf::Color::Green);
    playButton.setPosition(320.f, 400.f);

    scoreboardButton.setFont(font);
    scoreboardButton.setString("2. WYNIKI");
    scoreboardButton.setCharacterSize(35);
    scoreboardButton.setFillColor(sf::Color::Yellow);
    scoreboardButton.setPosition(320.f, 500.f);

    exitButton.setFont(font);
    exitButton.setString("3. WYJDZ");
    exitButton.setCharacterSize(35);
    exitButton.setFillColor(sf::Color::Red);
    exitButton.setPosition(320.f, 600.f);

    // --- NOWOŚĆ: IKONKA POMOCY "?" (Prawy górny róg: X=720, Y=30) ---
    helpButtonCircle.setRadius(20.f);
    helpButtonCircle.setFillColor(sf::Color(70, 70, 70));
    helpButtonCircle.setOutlineThickness(2.f);
    helpButtonCircle.setOutlineColor(sf::Color::White);
    helpButtonCircle.setPosition(720.f, 30.f);

    helpButtonText.setFont(font);
    helpButtonText.setString("?");
    helpButtonText.setCharacterSize(26);
    helpButtonText.setFillColor(sf::Color::White);
    helpButtonText.setStyle(sf::Text::Bold);
    // Wyśrodkowanie tekstu "?" w kółku o promieniu 20 (rozmiar 40x40)
    helpButtonText.setPosition(733.f, 33.f);

    // --- NOWOŚĆ: EKRAN INSTRUKCJI "JAK GRAĆ" ---
    helpTitle.setFont(font);
    helpTitle.setString("JAK GRAC?");
    helpTitle.setCharacterSize(45);
    helpTitle.setFillColor(sf::Color::Cyan);
    helpTitle.setPosition(300.f, 120.f);

    helpContent.setFont(font);
    helpContent.setCharacterSize(22);
    helpContent.setFillColor(sf::Color::White);
    helpContent.setPosition(80.f, 220.f);
    helpContent.setString(
        "Zasady gry sa proste! Nutki spadaja z gory ekranu.\n"
        "Twoim zadaniem jest wcisniecie odpowiedniego klawisza\n"
        "dokladnie w momencie, gdy nuta pokryje sie z receptorem.\n\n"
        "STEROWANIE:\n"
        " Sciezka 1 (Zielona)   ->  Klawisz [ S ]\n"
        " Sciezka 2 (Czerwona)  ->  Klawisz [ D ]\n"
        " Sciezka 3 (Zolta)     ->  Klawisz [ SPACJA ]\n"
        " Sciezka 4 (Niebieska) ->  Klawisz [ J ]\n"
        " Sciezka 5 (Rozowa)    ->  Klawisz [ K ]\n\n"
        "PAUZA:\n"
        " Podczas rozgrywki mozesz wcisnac [ P ], aby zatrzymac gre.\n\n"
        "OCENY TRAFIEN:\n"
        " - PERFECT! (do 45ms odchylki) - Najwiecej punktow!\n"
        " - SUPER!   (do 90ms odchylki)\n"
        " - GOOD     (do 180ms odchylki)\n"
        " - MISS     (powyzej 180ms lub brak klikniecia)\n\n"
        "W menu glownym mozesz suwakiem wlaczyc modyfikator chmur,\n"
        "ktory proceduralnie generuje chmury zaslaniajace nuty!"
        );

    // --- INICJALIZACJA WYBORU UTWORÓW ---
    selectTitle.setFont(font);
    selectTitle.setString("WYBIERZ UTWOR:");
    selectTitle.setCharacterSize(40);
    selectTitle.setFillColor(sf::Color::White);
    selectTitle.setPosition(250.f, 100.f);

    songFiles = {"song1.txt", "song2.txt", "song3.txt"};
    for (size_t i = 0; i < songFiles.size(); ++i) {
        sf::Text btn;
        btn.setFont(font);
        btn.setString(songFiles[i]);
        btn.setCharacterSize(30);
        btn.setFillColor(sf::Color::Cyan);
        btn.setPosition(300.f, 250.f + (i * 70.f));
        songButtons.push_back(btn);
    }

    backButton.setFont(font);
    backButton.setString("POWROT");
    backButton.setCharacterSize(30);
    backButton.setFillColor(sf::Color::White);
    backButton.setPosition(50.f, 50.f);
}

void Interface::drawMenu(sf::RenderWindow& window) {
    window.draw(menuTitle);
    window.draw(playButton);
    window.draw(scoreboardButton);
    window.draw(exitButton);

    // Rysowanie ikonki pomocy w menu
    window.draw(helpButtonCircle);
    window.draw(helpButtonText);
}
void Interface::drawHowToPlay(sf::RenderWindow& window) {
    window.draw(helpTitle);
    window.draw(helpContent);
    window.draw(backButton);
}

void Interface::drawSongSelect(sf::RenderWindow& window) {
    window.draw(selectTitle);
    for (const auto& btn : songButtons) {
        window.draw(btn);
    }
}

void Interface::drawEnterNickname(sf::RenderWindow& window, const std::string& currentNick) {
    sf::Text titleText("UTWOR UKONCZONY!", font, 40);
    titleText.setFillColor(sf::Color::Green);
    titleText.setPosition(220.f, 300.f);

    sf::Text inputInfo("WPISZ SWOJ NICK (3 LITERY):\n             " + currentNick, font, 30);
    inputInfo.setFillColor(sf::Color::White);
    inputInfo.setPosition(200.f, 420.f);

    sf::Text subText("[Wcisnij ENTER aby zapisac]", font, 20);
    subText.setFillColor(sf::Color::Yellow);
    subText.setPosition(270.f, 550.f);

    window.draw(titleText);
    window.draw(inputInfo);
    window.draw(subText);
}

void Interface::drawScoreboard(sf::RenderWindow& window, const ScoreManager& scoreManager) {
    sf::Text title("NAJLEPSZE WYNIKI", font, 45);
    title.setFillColor(sf::Color::Yellow);
    title.setPosition(220.f, 120.f);
    window.draw(title);
    window.draw(backButton);

    const auto& scores = scoreManager.getScores();
    for (size_t i = 0; i < scores.size() && i < 10; ++i) {
        sf::Text rowText(std::to_string(i + 1) + ".  " + scores[i].nickname + "    " + std::to_string(scores[i].score) + " pkt", font, 28);
        rowText.setFillColor(sf::Color::White);
        rowText.setPosition(280.f, 220.f + (i * 45.f));
        window.draw(rowText);
    }
    if(scores.empty()) {
        sf::Text emptyText("Brak zapisanych wynikow!", font, 24);
        emptyText.setFillColor(sf::Color::Cyan);
        emptyText.setPosition(270.f, 350.f);
        window.draw(emptyText);
    }
}

GameState Interface::handleMenuClick(const sf::Vector2i& mousePos) {
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

    // Sprawdzenie kliknięcia w ikonkę pomocy
    if (helpButtonCircle.getGlobalBounds().contains(mousePosF)) return GameState::HOW_TO_PLAY;

    if (playButton.getGlobalBounds().contains(mousePosF)) return GameState::SONG_SELECT;
    if (scoreboardButton.getGlobalBounds().contains(mousePosF)) return GameState::SCOREBOARD;
    if (exitButton.getGlobalBounds().contains(mousePosF)) exit(0);
    return GameState::MENU;
}

GameState Interface::handleHowToPlayClick(const sf::Vector2i& mousePos) {
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    if (backButton.getGlobalBounds().contains(mousePosF)) return GameState::MENU;
    return GameState::HOW_TO_PLAY;
}

std::string Interface::handleSongSelectClick(const sf::Vector2i& mousePos) {
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    for (size_t i = 0; i < songButtons.size(); ++i) {
        if (songButtons[i].getGlobalBounds().contains(mousePosF)) {
            return songFiles[i];
        }
    }
    return "";
}

GameState Interface::handleScoreboardClick(const sf::Vector2i& mousePos) {
    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
    if (backButton.getGlobalBounds().contains(mousePosF)) return GameState::MENU;
    return GameState::SCOREBOARD;
}