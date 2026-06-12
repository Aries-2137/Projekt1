#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "ScoreManager.h"

// Precyzyjna definicja typów stanów gry - upewnij się, że przecinki są na swoim miejscu!
enum class GameState {
    MENU,
    SONG_SELECT,
    PLAYING,
    ENTER_NICKNAME,
    SCOREBOARD
};

class Interface {
public:
    Interface();

    void drawMenu(sf::RenderWindow& window);
    void drawSongSelect(sf::RenderWindow& window);
    void drawEnterNickname(sf::RenderWindow& window, const std::string& currentNick);
    void drawScoreboard(sf::RenderWindow& window, const ScoreManager& scoreManager);

    GameState handleMenuClick(const sf::Vector2i& mousePos);
    std::string handleSongSelectClick(const sf::Vector2i& mousePos);
    GameState handleScoreboardClick(const sf::Vector2i& mousePos);

private:
    sf::Font font;
    sf::Text menuTitle;
    sf::Text playButton;
    sf::Text scoreboardButton;
    sf::Text exitButton;

    sf::Text selectTitle;
    std::vector<std::string> songFiles;
    std::vector<sf::Text> songButtons;

    sf::Text backButton;
};