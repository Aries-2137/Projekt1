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
    // std::cout << "Aktualny folder roboczy: " << std::filesystem::current_path() << std::endl;

    // 1. Inicjalizacja głównych modułów
    Game game("");
    ScoreManager scoreManager("scores.dat");
    InputHandler inputHandler;
    Interface interface;

    // Ładowanie czcionki do wyświetlania punktów
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Blad: Nie znaleziono czcionki arial.ttf!" << std::endl;
    }

    GameState currentState = GameState::MENU;
    scoreManager.displayScores();

    // 2. Tworzenie okna gry
    sf::RenderWindow window(sf::VideoMode(800, 1000), "AGH");
    window.setFramerateLimit(60);

    auto startTime = std::chrono::high_resolution_clock::now();
    auto previousTime = startTime;

    // 3. Pętla główna
    while (window.isOpen()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTimeDuration = currentTime - previousTime;
        float deltaTime = deltaTimeDuration.count();
        previousTime = currentTime;

        std::chrono::duration<float, std::milli> elapsedTime = currentTime - startTime;
        float currentTimeMs = elapsedTime.count();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                if (currentState == GameState::MENU) {
                    currentState = interface.handleMenuClick(mousePos);
                }
                else if (currentState == GameState::SONG_SELECT) {
                    if (currentState == GameState::SONG_SELECT) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                        // Te parametry MAJĄ być identyczne jak w Interface::drawSongSelect
                        float startX = 120.0f;
                        float startY = 250.0f;
                        float rectWidth = 240.0f;
                        float rectHeight = 60.0f;
                        float gapX = 80.0f;
                        float gapY = 40.0f;

                        const auto& songs = interface.getSongList();

                        for (size_t i = 0; i < songs.size() && i < 10; ++i) {
                            int col = i / 5;
                            int row = i % 5;

                            float xPos = startX + col * (rectWidth + gapX);
                            float yPos = startY + row * (rectHeight + gapY);

                            // Sprawdzenie kolizji myszy z przyciskiem
                            if (mousePos.x >= xPos && mousePos.x <= xPos + rectWidth &&
                                mousePos.y >= yPos && mousePos.y <= yPos + rectHeight)
                            {
                                std::string selectedSong = songs[i];


                                game.loadSongSequence(selectedSong);

                                // Reset czasu i stanu gry
                                startTime = std::chrono::high_resolution_clock::now();
                                previousTime = startTime;
                                currentState = GameState::PLAYING;

                                std::cout << "Wybrano utwor: " << selectedSong << std::endl;
                                break;
                            }
                        }
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
                        inputHandler.handleKeyPress(keypressed, currentTimeMs, game.getNotes(), game);
                    }
                }
                else if (event.type == sf::Event::KeyReleased) {
                    char keyreleased = '\0';
                    if (event.key.code == sf::Keyboard::S) keyreleased = 'S';
                    else if (event.key.code == sf::Keyboard::D) keyreleased = 'D';
                    else if (event.key.code == sf::Keyboard::Space) keyreleased = ' ';
                    else if (event.key.code == sf::Keyboard::J) keyreleased = 'J';
                    else if (event.key.code == sf::Keyboard::K) keyreleased = 'K';

                    if (keyreleased != '\0') {
                        inputHandler.handleKeyRelease(keyreleased, currentTimeMs, game);
                    }
                }
            }
        }

        // 4. Logika i renderowanie
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

            // Rysowanie wyniku w prawym górnym rogu
            sf::Text scoreText;
            scoreText.setFont(font);
            scoreText.setString("Punkty: " + std::to_string(inputHandler.getTotalScore()));
            scoreText.setCharacterSize(24);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setPosition(620.f, 20.f);
            window.draw(scoreText);

            if (currentTimeMs > 30000.0f) { //czas trwania utworu
                int finalScore = inputHandler.getTotalScore();
                std::cout << "\nKoniec utworu! Wynik: " << finalScore << std::endl;
                scoreManager.addScore(finalScore);
                currentState = GameState::MENU;
            }
        }

        window.display();
    }
    return 0;
}