#include <iostream>
#include <chrono>
#include <filesystem>
#include <SFML/Graphics.hpp>
#include "Game.h"
#include "InputHandler.h"
#include "ScoreManager.h"
#include "Interface.h"
#include "Conductor.h" // <--- Dodano nagłówek Conductora

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

            // POPRAWKA: Przechwytywanie kliknięć klawiszy zsynchronizowane z muzyką
            if (event.type == sf::Event::KeyPressed) {
                char keyPressedLocal = '\0';
                if (event.key.code == sf::Keyboard::S) keyPressedLocal = 'S';
                else if (event.key.code == sf::Keyboard::D) keyPressedLocal = 'D';
                else if (event.key.code == sf::Keyboard::Space) keyPressedLocal = ' ';
                else if (event.key.code == sf::Keyboard::J) keyPressedLocal = 'J';
                else if (event.key.code == sf::Keyboard::K) keyPressedLocal = 'K';

                if (keyPressedLocal != '\0') {
                    // Jeśli gramy, przekazujemy pozycję piosenki z Conductora, w menu zwykły czas aplikacji
                    float gameplayTime = (currentState == GameState::PLAYING) ? Conductor::songPosition : currentTimeMs;
                    inputHandler.handleKeyPress(keyPressedLocal, gameplayTime, game.getNotes(), game);
                }
            }

            if (event.type == sf::Event::KeyReleased) {
                char keyreleasedLocal = '\0';
                if (event.key.code == sf::Keyboard::S) keyreleasedLocal = 'S';
                else if (event.key.code == sf::Keyboard::D) keyreleasedLocal = 'D';
                else if (event.key.code == sf::Keyboard::Space) keyreleasedLocal = ' ';
                else if (event.key.code == sf::Keyboard::J) keyreleasedLocal = 'J';
                else if (event.key.code == sf::Keyboard::K) keyreleasedLocal = 'K';

                if (keyreleasedLocal != '\0') {
                    float gameplayTime = (currentState == GameState::PLAYING) ? Conductor::songPosition : currentTimeMs;
                    inputHandler.handleKeyRelease(keyreleasedLocal, gameplayTime, game);
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (currentState == GameState::MENU) {
                        currentState = interface.handleMenuClick(mousePos);
                    }
                    else if (currentState == GameState::SONG_SELECT) {
                        std::string selectedSong = interface.handleSongSelectClick(mousePos);
                        if (!selectedSong.empty()) {
                            game.loadNewSong(selectedSong);
                            currentState = GameState::PLAYING;
                        }
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