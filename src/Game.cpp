#include "Game.h"
#include "Note.h"
#include "NoteType.h"
#include "Conductor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

Game::Game(const std::string& songFilename)
    : currentSongFile(songFilename), lastUpdatedTime(0.0f)
{
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Blad: Nie udalo sie wczytac czcionki arial.ttf do gry!\n";
    }

    for(int i = 0; i < 5; ++i) {
        receptorScales[i] = 1.0f;
        laneHighlightEndTime[i] = 0.0f;
    }
    loadSongSequence(currentSongFile);
}

Game::~Game() {
    song.stop();
}

void Game::loadSongSequence(const std::string& filename) {
    if (filename.empty()) return;

    std::string fullPath = "music_sequence/" + filename;
    std::ifstream file(fullPath);
    if (!file.is_open()) return;

    std::string line;
    float songBpm = 120.0f;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line.find("BPM") != std::string::npos) {
            std::string clean = "";
            for (char c : line) {
                if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') clean += c;
            }
            if (!clean.empty()) songBpm = std::stof(clean);
            continue;
        }

        for (char &c : line) {
            if (!std::isdigit(static_cast<unsigned char>(c)) && c != ' ' && c != '-' && c != '\t') c = ' ';
        }

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

    song.stop();
    std::string audioPath = "music_sequence/" + filename;
    size_t extPos = audioPath.find_last_of(".");
    if (extPos != std::string::npos) audioPath = audioPath.substr(0, extPos) + ".ogg";

    if (song.openFromFile(audioPath)) {
        song.setVolume(50.0f);
        song.play();
        Conductor::initialize(songBpm, 0.0f);
    }
}

void Game::loadNewSong(const std::string& filename) {
    activeNotes.clear();
    allNotes.clear();
    feedbacks.clear();
    song.stop();
    currentSongFile = filename;
    loadSongSequence(currentSongFile);
}

void Game::update(float deltaTime, float currentTimeMs) {
    Conductor::update(song);
    float audioTimeMs = Conductor::songPosition;
    lastUpdatedTime = audioTimeMs;

    // Aktualizacja i płynny powrót skali receptorów do normy (1.0)
    for (int i = 0; i < 5; ++i) {
        // ZASADA BEAT TRIGGERA: Jeśli Conductor wykrył uderzenie, dodaj efekt pulsu
        if (Conductor::justTriggeredBeat) {
            receptorScales[i] += 0.15f;
            if (receptorScales[i] > 1.4f) receptorScales[i] = 1.4f;
        }

        if (receptorScales[i] > 1.0f) {
            receptorScales[i] -= 4.0f * deltaTime; // Powrót do standardowego rozmiaru
            if (receptorScales[i] < 1.0f) receptorScales[i] = 1.0f;
        }
    }

    auto it = allNotes.begin();
    while (it != allNotes.end()) {
        if (audioTimeMs >= (*it)->getTargetTime() - 1500.0f) {
            activeNotes.push_back(std::move(*it));
            it = allNotes.erase(it);
        } else {
            ++it;
        }
    }

    for (auto& note : activeNotes) {
        note->updateSynced(audioTimeMs);
    }

    activeNotes.erase(std::remove_if(activeNotes.begin(), activeNotes.end(),
                                     [](const std::unique_ptr<Note>& note) {
                                         return note->getY() > 1100.0f;
                                     }), activeNotes.end());

    for (auto fbIt = feedbacks.begin(); fbIt != feedbacks.end(); ) {
        fbIt->timer -= deltaTime;
        fbIt->y -= 50.0f * deltaTime;
        if (fbIt->timer <= 0.0f) fbIt = feedbacks.erase(fbIt);
        else ++fbIt;
    }
}

void Game::draw(sf::RenderWindow& window) {
    float startX = 150.0f;
    float spacing = 100.0f;


    float hitZoneY = 750.0f;
    float circleRadius = 35.0f;
    float rectWidth = 90.0f;
    float rectHeight = 30.0f;

    int lineBrightness = static_cast<int>(50.0f + (1.0f - Conductor::beatProgress) * 40.0f);
    sf::Color linePulseColor(lineBrightness, lineBrightness, lineBrightness);

    for (int i = 1; i <= 5; ++i) {
        float xPos = startX + (i - 1) * spacing;


        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(xPos, 0.0f), linePulseColor),
            sf::Vertex(sf::Vector2f(xPos, 1000.0f), linePulseColor)
        };
        window.draw(line, 2, sf::Lines);


        sf::CircleShape receptor(circleRadius);
        receptor.setOrigin(circleRadius, circleRadius);
        receptor.setPosition(xPos, hitZoneY);
        receptor.setFillColor(sf::Color::Transparent);
        receptor.setOutlineThickness(4.0f);
        receptor.setScale(receptorScales[i - 1], receptorScales[i - 1]);

        switch (i) {
        case 1: receptor.setOutlineColor(sf::Color::Green); break;
        case 2: receptor.setOutlineColor(sf::Color::Red); break;
        case 3: receptor.setOutlineColor(sf::Color::Yellow); break;
        case 4: receptor.setOutlineColor(sf::Color::Blue); break;
        case 5: receptor.setOutlineColor(sf::Color::Magenta); break;
        }
        window.draw(receptor);

        // Rysowanie małych prostokątów podpowiedzi (podświetlenie linii)
        sf::RectangleShape hintRect(sf::Vector2f(rectWidth, rectHeight));
        hintRect.setOrigin(rectWidth / 2.0f, rectHeight / 2.0f);
        hintRect.setPosition(xPos, hitZoneY + 60.0f); // Obniżone lekko pod kółko

        if (lastUpdatedTime < laneHighlightEndTime[i - 1]) {
            hintRect.setFillColor(sf::Color::White);
        } else {
            hintRect.setFillColor(sf::Color(100, 100, 100));
        }
        window.draw(hintRect);
    }

    for (const auto& note : activeNotes) note->draw(window);

    for (const auto& fb : feedbacks) {
        sf::Text textInfo(fb.text, font, 24);
        textInfo.setPosition(fb.x, fb.y);
        textInfo.setFillColor(fb.color);
        window.draw(textInfo);
    }

    sf::Text infoText;
    infoText.setFont(font);
    infoText.setString("BEAT: " + std::to_string(Conductor::currentBeat) + "  |  BPM: " + std::to_string(static_cast<int>(Conductor::bpm)));
    infoText.setCharacterSize(22);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition(20.0f, 20.0f);
    window.draw(infoText);
}

void Game::triggerReceptorAnimation(int lane) {
    if (lane >= 1 && lane <= 5) {
        receptorScales[lane - 1] = 1.35f; // Silniejszy impuls przy trafnym kliknięciu gracza
        laneHighlightEndTime[lane - 1] = lastUpdatedTime + 150.0f;
    }
}

void Game::addFeedback(int lane, const std::string& text, sf::Color color) {
    if (lane >= 1 && lane <= 5) {
        float xPos = 150.0f + (lane - 1) * 100.0f - 40.0f;
        feedbacks.push_back({ text, color, 0.5f, xPos, 750.0f });
    }
}
