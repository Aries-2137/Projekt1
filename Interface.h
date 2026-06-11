#ifndef INTERFACE_H
#define INTERFACE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

enum class GameState {
    MENU,
    SONG_SELECT,
    PLAYING
};

class Interface {
public:
    Interface();

    // Rysowanie poszczególnych ekranów
    void drawMenu(sf::RenderWindow& window);
    void drawSongSelect(sf::RenderWindow& window);

    // Obsługa kliknięć myszką
    GameState handleMenuClick(const sf::Vector2i& mousePos);
    std::string handleSongSelectClick(const sf::Vector2i& mousePos);

private:
    sf::Font font;

    // Teksty Menu Głównego
    sf::Text menuTitle;
    sf::Text playButton;
    sf::Text exitButton;

    // Teksty Wyboru Utworów
    sf::Text selectTitle;
    std::vector<std::string> songFiles; // Lista nazw plików .txt
    std::vector<sf::Text> songButtons;  // Graficzne napisy piosenek
};

#endif // INTERFACE_H