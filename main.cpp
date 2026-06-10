#include <iostream>
#include <chrono>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include "Game.h"
#include "InputHandler.h"
#include "ScoreManager.h"
#include "Interface.h"

int main() {
    std::cout << "--- Start Gry: AGH ---\n";

    Game game("");
    ScoreManager scoreManager("scores.dat");
    InputHandler inputHandler;
    Interface interface;

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Blad: Nie znaleziono czcionki arial.ttf!" << std::endl;
    }

    GameState currentState = GameState::MENU;
    scoreManager.displayScores();

    sf::RenderWindow window(sf::VideoMode(800, 1000), "AGH");
    window.setFramerateLimit(60);

    auto startTime = std::chrono::high_resolution_clock::now();
    auto previousTime = startTime;

    while (window.isOpen()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTimeDuration = currentTime - previousTime;
        float deltaTime = deltaTimeDuration.count();
        previousTime = currentTime;

        std::chrono::duration<float, std::milli> elapsed = currentTime - startTime;
        float currentTimeMs = elapsed.count();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                // MYSZKA LOG: Wypisuje współrzędne każdego kliknięcia
                std::cout << "[MYSZKA]: Kliknieto X: " << mousePos.x << ", Y: " << mousePos.y << " | Aktualny Stan Gry: " << (int)currentState << std::endl;

                if (currentState == GameState::MENU) {
                    currentState = interface.handleMenuClick(mousePos);
                }
                else if (currentState == GameState::SONG_SELECT) {
                    std::string selectedSong = interface.handleSongSelectClick(mousePos);

                    // INTERFEJS LOG: Pokazuje, co funkcja z Interface.cpp przekazała do silnika gry
                    std::cout << "[INTERFEJS]: handleSongSelectClick zwrocil nazwe: '" << selectedSong << "'\n";

                    if (!selectedSong.empty()) {
                        game.loadNewSong(selectedSong);

                        startTime = std::chrono::high_resolution_clock::now();
                        previousTime = startTime;
                        currentState = GameState::PLAYING;
                    }
                }
            }

            if (currentState == GameState::PLAYING) {
                if (event.type == sf::Event::KeyPressed) {
                    char keypressed = '\0';
                    if (event.key.code == sf::Keyboard::S) keypressed = 'S';
                    else if (event.key.code == sf::Keyboard::D) keypressed = 'D';
                    else if (event.key.code == sf::Keyboard::Space) keypressed = ' ';
                    else if (event.key.code == sf::Keyboard::J) keypressed = 'J';
                    else if (event.key.code == sf::Keyboard::K) keypressed = 'K';

                    if (keypressed != '\0') {
                        game.triggerReceptorAnimation(keypressed == 'S' ? 1 : keypressed == 'D' ? 2 : keypressed == ' ' ? 3 : keypressed == 'J' ? 4 : 5);
                        inputHandler.handleKeyPress(keypressed, currentTimeMs, game.getNotes(), game);
                    }
                }
                else if (event.type == sf::Event::KeyReleased) {
                    char keyreleasedLocal = '\0';
                    if (event.key.code == sf::Keyboard::S) keyreleasedLocal = 'S';
                    else if (event.key.code == sf::Keyboard::D) keyreleasedLocal = 'D';
                    else if (event.key.code == sf::Keyboard::Space) keyreleasedLocal = ' ';
                    else if (event.key.code == sf::Keyboard::J) keyreleasedLocal = 'J';
                    else if (event.key.code == sf::Keyboard::K) keyreleasedLocal = 'K';

                    if (keyreleasedLocal != '\0') {
                        inputHandler.handleKeyRelease(keyreleasedLocal, currentTimeMs, game);
                    }
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));

        if (currentState == GameState::MENU) {
            interface.drawMenu(window);
        }
        else if (currentState == GameState::SONG_SELECT) {
            interface.drawSongSelect(window);
        }
        else if (currentState == GameState::PLAYING) {
            game.update(deltaTime, currentTimeMs);
            game.draw(window);

            sf::Text scoreText;
            scoreText.setFont(font);
            scoreText.setString("Punkty: " + std::to_string(inputHandler.getTotalScore()));
            scoreText.setCharacterSize(24);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setPosition(620.f, 20.f);
            window.draw(scoreText);

            // Gra skończy się dopiero wtedy, kiedy piosenka dobiegnie do końca na dysku
            if (currentState == GameState::PLAYING && game.isSongFinished()) {
                int finalScore = inputHandler.getTotalScore();
                std::cout << "\nKoniec utworu! Wynik: " << finalScore << std::endl;

                game.loadNewSong("");
                scoreManager.addScore(finalScore);
                currentState = GameState::MENU;
            }
        }

        window.display();
    }

    return 0;
}