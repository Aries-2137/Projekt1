#include <iostream>
#include <chrono>
#include <SFML/Graphics.hpp>
#include "Game.h"
#include "InputHandler.h"
#include "ScoreManager.h"
#include "Interface.h"
#include "Conductor.h"
#include "CloudModifier.h"

int main() {
    std::cout << "--- Start Gry: AGH ---\n";

    Game game("");
    CloudModifier cloudModifier;
    ScoreManager scoreManager("scores.dat");
    InputHandler inputHandler;
    Interface interface;

    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cerr << "Blad: Nie znaleziono czcionki arial.ttf!" << std::endl;
    }

    std::string playerNick = "";
    GameState currentState = GameState::MENU;

    sf::RenderWindow window(sf::VideoMode(800, 1000), "AGH Rhythm Game");
    window.setFramerateLimit(60);

    auto previousTime = std::chrono::high_resolution_clock::now();
    bool isSongFiredUp = false;

    while (window.isOpen()) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - previousTime).count();
        previousTime = currentTime;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // 1. WPISYWANIE NICKU
            if (currentState == GameState::ENTER_NICKNAME) {
                if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode >= 32 && event.text.unicode < 128) {
                        if (playerNick.size() < 3) {
                            playerNick += std::toupper(static_cast<char>(event.text.unicode));
                        }
                    }
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                    if (playerNick.size() == 3) {
                        scoreManager.addScore(playerNick, inputHandler.getTotalScore());
                        playerNick = "";
                        currentState = GameState::SCOREBOARD;
                    }
                }
            }

            // 2. KLAWIATURA W GRZE
            if (currentState == GameState::PLAYING) {
                if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
                    char key = '\0';
                    if (event.key.code == sf::Keyboard::S) key = 'S';
                    else if (event.key.code == sf::Keyboard::D) key = 'D';
                    else if (event.key.code == sf::Keyboard::Space) key = ' ';
                    else if (event.key.code == sf::Keyboard::J) key = 'J';
                    else if (event.key.code == sf::Keyboard::K) key = 'K';

                    if (key != '\0') {
                        if (event.type == sf::Event::KeyPressed) {
                            // POPRAWKA: Przekazujemy game.getActiveNotes() zamiast game.getNotes()
                            inputHandler.handleKeyPress(key, Conductor::songPosition, game.getActiveNotes(), game);
                        }
                        else {
                            inputHandler.handleKeyRelease(key, Conductor::songPosition, game);
                        }
                    }
                }
            }

            // 3. MYSZKA
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (currentState == GameState::MENU) {
                    game.getCloudModifier().handleMouseClick(mousePos);

                    GameState nextState = interface.handleMenuClick(mousePos);
                    if (nextState == GameState::SONG_SELECT || nextState == GameState::PLAYING) {
                        inputHandler.resetCombo();
                        game.loadNewSong("muzyka1.txt");
                        isSongFiredUp = false;
                        currentState = GameState::PLAYING;
                    } else {
                        currentState = nextState;
                    }
                }
                else if (currentState == GameState::SCOREBOARD) {
                    currentState = interface.handleScoreboardClick(mousePos);
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                if (currentState == GameState::MENU) {
                    game.getCloudModifier().handleMouseRelease();
                }
            }

            // Ruch myszką (PRZESUWANIE SUWAKA W CZASIE RZECZYWISTYM)
            else if (event.type == sf::Event::MouseMoved) {
                if (currentState == GameState::MENU) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    game.getCloudModifier().handleMouseMove(mousePos);
                }
            }
        }

        // --- RENDEROWANIE ---
        // --- RENDEROWANIE ---
        window.clear(sf::Color(30, 30, 30));

        if (currentState == GameState::MENU) {
            interface.drawMenu(window);
            game.getCloudModifier().drawButton(window); // <-- TUTAJ: Rysujemy sam przycisk w menu głównym
        }
        else if (currentState == GameState::SCOREBOARD) {
            interface.drawScoreboard(window, scoreManager);
        }
        else if (currentState == GameState::ENTER_NICKNAME) {
            interface.drawEnterNickname(window, playerNick);
        }
        else if (currentState == GameState::PLAYING) {
            game.update(deltaTime, Conductor::songPosition);
            cloudModifier.update(deltaTime); // Aktualizacja fizyki chmur

            game.draw(window);
            cloudModifier.draw(window); // <-- TUTAJ: Rysujemy chmury nad nutami (bez przycisku)

            // Rysowanie punktacji
            sf::Text scoreText("Punkty: " + std::to_string(inputHandler.getTotalScore()), font, 24);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setPosition(620.f, 20.f);
            window.draw(scoreText);

            if (!game.isSongFinished()) {
                isSongFiredUp = true;
            }

            if (isSongFiredUp && game.isSongFinished()) {
                std::cout << "Koniec utworu! Wynik: " << inputHandler.getTotalScore() << std::endl;
                currentState = GameState::ENTER_NICKNAME;
            }
        }

        window.display();
    }

    return 0;
}