#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum class GameState { MENU, SONG_SELECT, PLAYING };

class Interface {
private:
    sf::Font font;
    std::vector<std::string> songList = {};
    sf::Text menuText[3]; // "Utwory", "Wyniki", "Wyjdź"

public:
    Interface();
    void drawMenu(sf::RenderWindow& window);
    void drawSongSelect(sf::RenderWindow& window);

    // Zwraca stan gry po kliknięciu
    GameState handleMenuClick(sf::Vector2i mousePos);
    // Zwraca nazwę pliku po kliknięciu w utwór
    std::string handleSongSelectClick(sf::Vector2i mousePos);
    const std::vector<std::string>& getSongList() const { return songList; }
};