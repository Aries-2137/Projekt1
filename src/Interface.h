// Zmiana w pliku Interface.h

#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "ScoreManager.h"

enum class GameState {
    MENU,
    SONG_SELECT,
    PLAYING,
    ENTER_NICKNAME,
    SCOREBOARD,
    HOW_TO_PLAY // <-- NOWY STAN GRY
};

class Interface {
public:
    Interface();

    void drawMenu(sf::RenderWindow& window);
    void drawSongSelect(sf::RenderWindow& window);
    void drawEnterNickname(sf::RenderWindow& window, const std::string& currentNick);
    void drawScoreboard(sf::RenderWindow& window, const ScoreManager& scoreManager);
    void drawHowToPlay(sf::RenderWindow& window); // <-- NOWA METODA

    GameState handleMenuClick(const sf::Vector2i& mousePos);
    std::string handleSongSelectClick(const sf::Vector2i& mousePos);
    GameState handleScoreboardClick(const sf::Vector2i& mousePos);
    GameState handleHowToPlayClick(const sf::Vector2i& mousePos); // <-- NOWA METODA

private:
    sf::Font font;
    sf::Text menuTitle;
    sf::Text playButton;
    sf::Text scoreboardButton;
    sf::Text exitButton;

    // Elementy dla przycisku pomocy w menu
    sf::CircleShape helpButtonCircle; // Kółko tła dla ikonki "?"
    sf::Text helpButtonText;         // Znak "?" wewnątrz kółka

    // Teksty ekranu instrukcji
    sf::Text helpTitle;
    sf::Text helpContent;

    sf::Text selectTitle;
    std::vector<std::string> songFiles;
    std::vector<sf::Text> songButtons;

    sf::Text backButton;
};