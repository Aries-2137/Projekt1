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
    : currentSongFile(songFilename), lastUpdatedTime(0.0f), hasStarted(false)
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
    if (!file.is_open()) {
        std::cerr << "BLAD: Nie mozna otworzyc pliku: " << fullPath << std::endl;
        return;
    }

    allNotes.clear();
    std::string line;
    float songBpm = 120.0f; // Domyślne BPM

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // Jeśli linia zawiera "BPM", parsujemy liczbę
        if (line.find("BPM") != std::string::npos) {
            std::stringstream ss(line);
            std::string temp;
            ss >> temp >> songBpm; // Pomija słowo "BPM", wczytuje wartość liczbową
            std::cout << "Wykryto BPM w pliku: " << songBpm << std::endl;
        }
        // W przeciwnym razie traktujemy linię jako nutę: "lane time duration"
        else {
            std::stringstream ss(line);
            int lane, time, duration;
            if (ss >> lane >> time >> duration) {
                if (duration == 1) allNotes.push_back(std::make_unique<NoteType1>(lane, time, 1.0f, 0.0f));
                else if (duration == 2) allNotes.push_back(std::make_unique<NoteType2>(lane, time, 1.0f, 0.0f));
                else if (duration == 3) allNotes.push_back(std::make_unique<NoteType3>(lane, time, 1.0f, 0.0f));
                else if (duration == 4) allNotes.push_back(std::make_unique<NoteType4>(lane, time, 1.0f, 0.0f));
                else if (duration == 5) allNotes.push_back(std::make_unique<NoteType5>(lane, time, 1.0f, 0.0f));
            }
        }
    }

    // Ważne: Po wczytaniu nut, przekazujemy BPM do Conductora
    Conductor::initialize(songBpm, 0.0f);
    std::cout << "Wczytano " << allNotes.size() << " nut." << std::endl;
}

void Game::loadNewSong(const std::string& filename) {
    if (filename.empty()) {
        song.stop();
        hasStarted = false;
        return;
    }

    // 1. Wczytaj nuty i BPM
    loadSongSequence(filename);

    // 2. Przygotuj plik audio (.ogg)
    std::string audioFilename = filename;
    size_t lastDot = audioFilename.find_last_of(".");
    if (lastDot != std::string::npos) {
        audioFilename = audioFilename.substr(0, lastDot) + ".ogg";
    }

    std::string fullPath = "music_sequence/" + audioFilename;
    song.stop();
    hasStarted = false;

    if (!song.openFromFile(fullPath)) {
        std::cerr << "BLAD: Nie mozna otworzyc audio: " << fullPath << std::endl;
        return;
    }

    song.play();
    hasStarted = true;
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
    float hitZoneY = 850.0f;
    float rectWidth = 90.0f;
    float rectHeight = 30.0f;

    // ZASADA BEAT PROGRESS: Wyznaczanie pulsującej jasności linii (zanikanie od uderzenia)
    int lineBrightness = static_cast<int>(50.0f + (1.0f - Conductor::beatProgress) * 40.0f);
    sf::Color linePulseColor(lineBrightness, lineBrightness, lineBrightness);

    for (int i = 1; i <= 5; ++i) {
        float xPos = startX + (i - 1) * spacing;

        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(xPos, 0.0f), linePulseColor),
            sf::Vertex(sf::Vector2f(xPos, 1000.0f), linePulseColor)
        };
        window.draw(line, 2, sf::Lines);

        sf::RectangleShape receptor(sf::Vector2f(rectWidth, rectHeight));
        receptor.setOrigin(rectWidth / 2.0f, rectHeight / 2.0f);
        receptor.setPosition(xPos, hitZoneY);
        receptor.setFillColor(sf::Color::Transparent);
        receptor.setOutlineThickness(3.0f);
        receptor.setScale(receptorScales[i - 1], receptorScales[i - 1]);

        switch (i) {
        case 1: receptor.setOutlineColor(sf::Color::Green); break;
        case 2: receptor.setOutlineColor(sf::Color::Red); break;
        case 3: receptor.setOutlineColor(sf::Color::Yellow); break;
        case 4: receptor.setOutlineColor(sf::Color::Blue); break;
        case 5: receptor.setOutlineColor(sf::Color::Magenta); break;
        }
        window.draw(receptor);

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