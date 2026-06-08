#include "Game.h"
#include "Note.h"
#include "NoteType.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>


Game::Game(const std::string& songFilename)
    : currentSongFile(songFilename), currentScore(0), comboMultiplier(1)
{
    font.loadFromFile("arial.ttf");

    for(int i = 0; i < 5; ++i) {
        receptorScales[i] = 1.0f;
        laneHighlightEndTime[i] = 0.0f;
    }
    loadSongSequence(currentSongFile);
}

Game::~Game() {}

void Game::loadSongSequence(const std::string& filename) {
    std::string fullPath = "music_sequence/" + filename;
    std::ifstream file(fullPath);

    if (!file.is_open()) {
        // std::cerr << "Blad: Nie znaleziono pliku w: " << fullPath << "\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        for (char &c : line) { if (c == ',' || c == ';') c = ' '; }

        std::stringstream ss(line);
        int lane, time, duration;

        if (ss >> lane >> time >> duration) {
            float speed = 450.0f;
            float rotSpeed = 90.0f;

            if (duration == 1) allNotes.push_back(std::make_unique<NoteType1>(lane, time, speed, rotSpeed));
            else if (duration == 2) allNotes.push_back(std::make_unique<NoteType2>(lane, time, speed, rotSpeed));
            else if (duration == 3) allNotes.push_back(std::make_unique<NoteType3>(lane, time, speed, rotSpeed));
            else if (duration == 4) allNotes.push_back(std::make_unique<NoteType4>(lane, time, speed, rotSpeed));
            else if (duration == 5) allNotes.push_back(std::make_unique<NoteType5>(lane, time, speed, rotSpeed));
        }
    }
    file.close();
}

void Game::update(float deltaTime, float currentTimeMs) {
    lastUpdatedTime = currentTimeMs;
    for (auto it = allNotes.begin(); it != allNotes.end(); ) {
        if ((*it)->getTargetTime() <= currentTimeMs + 2000.0f) {
            activeNotes.push_back(std::move(*it));
            it = allNotes.erase(it);
        } else {
            ++it;
        }
    }

    for (auto& note : activeNotes) {
        note->update(deltaTime);

        // Logika podświetlania (błysk 30ms przed trafieniem)
        float diff = std::abs(note->getTargetTime() - currentTimeMs);
        if (diff < 30.0f) {
            laneHighlightEndTime[note->getLane() - 1] = currentTimeMs + 50.0f;
        }
    }

    activeNotes.erase(std::remove_if(activeNotes.begin(), activeNotes.end(),
                                     [](const std::unique_ptr<Note>& note) { return note->getY() > 950.0f; }),
                      activeNotes.end());

    for (int i = 0; i < 5; ++i) {
        if (receptorScales[i] > 1.0f) {
            receptorScales[i] -= 3.0f * deltaTime;
            if (receptorScales[i] < 1.0f) receptorScales[i] = 1.0f;
        }
    }

    for (auto it = feedbacks.begin(); it != feedbacks.end(); ) {
        it->timer -= deltaTime;
        it->y -= 50.0f * deltaTime; // Napis powoli unosi się do góry
        if (it->timer <= 0.0f) {
            it = feedbacks.erase(it);
        } else {
            ++it;
        }
    }

}

void Game::triggerReceptorAnimation(int lane) {
    if (lane >= 1 && lane <= 5) {
        receptorScales[lane - 1] = 1.2f;
    }
}

void Game::draw(sf::RenderWindow& window) {
    float hitZoneY = 800.0f;
    float rectWidth = 60.0f;
    float rectHeight = 10.0f;

    // Dodajemy wizualizację okna trafienia (hit window)
    float hitWindowVisualHeight = 150.0f; // Odpowiada Twojemu minDiff = 150.0f

    for (int i = 1; i <= 5; ++i) {
        float xPos = 50.0f + (i * 100.0f);

        // 1. Rysowanie "okna trafienia" (debug zone)
        sf::RectangleShape hitZoneRect(sf::Vector2f(rectWidth, hitWindowVisualHeight));
        hitZoneRect.setOrigin(rectWidth / 2.0f, hitWindowVisualHeight / 2.0f);
        hitZoneRect.setPosition(xPos, hitZoneY);
        hitZoneRect.setFillColor(sf::Color(255, 255, 255, 30)); // Delikatna przezroczystość
        hitZoneRect.setOutlineColor(sf::Color(255, 255, 255, 100));
        hitZoneRect.setOutlineThickness(1.0f);
        window.draw(hitZoneRect);

        // 2. Rysowanie receptora
        float scale = receptorScales[i - 1];
        sf::CircleShape receptor(25.0f * scale);
        receptor.setOrigin(25.0f * scale, 25.0f * scale);
        receptor.setPosition(xPos, hitZoneY);
        receptor.setFillColor(sf::Color::Transparent);
        receptor.setOutlineThickness(3.0f * scale);

        switch (i) {
        case 1: receptor.setOutlineColor(sf::Color::Green); break;
        case 2: receptor.setOutlineColor(sf::Color::Red); break;
        case 3: receptor.setOutlineColor(sf::Color::Yellow); break;
        case 4: receptor.setOutlineColor(sf::Color::Blue); break;
        case 5: receptor.setOutlineColor(sf::Color::Magenta); break;
        }
        window.draw(receptor);

        // 3. Prostokąt podpowiedzi
        sf::RectangleShape hintRect(sf::Vector2f(rectWidth, rectHeight));
        hintRect.setOrigin(rectWidth / 2.0f, rectHeight / 2.0f);
        hintRect.setPosition(xPos, hitZoneY + 50.0f);

        if (lastUpdatedTime < laneHighlightEndTime[i - 1]) {
            hintRect.setFillColor(sf::Color::White);
        } else {
            hintRect.setFillColor(sf::Color(100, 100, 100));
        }
        window.draw(hintRect);
    }

    for (const auto& note : activeNotes) {
        note->draw(window);
    }

    for (const auto& fb : feedbacks) {
        sf::Text textInfo(fb.text, font, 24);
        textInfo.setPosition(fb.x, fb.y);
        textInfo.setFillColor(fb.color);
        window.draw(textInfo);
    }
}

void Game::loadNewSong(const std::string& filename) {
    activeNotes.clear();
    allNotes.clear();
    currentSongFile = filename;
    loadSongSequence(currentSongFile);
}

void Game::addFeedback(int lane, const std::string& text, sf::Color color) {
    float xPos = 30.0f + (lane * 100.0f);
    feedbacks.push_back({text, color, 0.5f, xPos, 700.0f}); // Tekst wisi przez 0.5 sekundy
}