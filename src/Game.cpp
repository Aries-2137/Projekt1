#include "Game.h"
#include "Conductor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include "InputHandler.h"

Game::Game(const std::string& dummy) : lastUpdatedTime(0.0f) {
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cerr << "[BLAD GRY]: Nie znaleziono czcionki arial.ttf!\n";
    }

    isPaused = false;

    pauseText.setFont(font);
    pauseText.setString("PAUZA");
    pauseText.setCharacterSize(60);
    pauseText.setFillColor(sf::Color::Red);
    pauseText.setOutlineThickness(3.0f);
    pauseText.setOutlineColor(sf::Color::Black);

    sf::FloatRect pRect = pauseText.getLocalBounds();
    pauseText.setOrigin(pRect.left + pRect.width / 2.0f, pRect.top + pRect.height / 2.0f);
    pauseText.setPosition(400.0f, 400.0f);

    infoText.setFont(font);
    infoText.setCharacterSize(24);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition(20.f, 20.f);



    for (int i = 0; i < 6; ++i) {
        receptorScales[i] = 1.0f;
    }
}

void Game::loadSongSequence(const std::string& filename) {
    if (filename.empty()) return;

    std::string fullPath = "assets/music_sequence/" + filename;
    std::ifstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "[BLAD WERYFIKACJI]: Nie znaleziono pliku mapy (.txt) pod sciezka wzgledna: " << fullPath << std::endl;
        return;
    }

    allNotes.clear();
    activeNotes.clear();

    std::string line;
    float songBpm = 120.0f;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string firstToken;
        ss >> firstToken;

        if (firstToken == "BPM") {
            ss >> songBpm;
        } else {
            try {
                int lane = std::stoi(firstToken);
                int targetTime, type;

                if (ss >> targetTime >> type) {
                    float defaultSpeed = 2.0f;
                    float defaultRotationSpeed = 50.0f;

                    if (type == 1) allNotes.push_back(std::make_unique<NoteType1>(lane, targetTime, defaultSpeed, defaultRotationSpeed));
                    else if (type == 2) allNotes.push_back(std::make_unique<NoteType2>(lane, targetTime, defaultSpeed, defaultRotationSpeed));
                    else if (type == 3) allNotes.push_back(std::make_unique<NoteType3>(lane, targetTime, defaultSpeed, defaultRotationSpeed));
                    else if (type == 4) allNotes.push_back(std::make_unique<NoteType4>(lane, targetTime, defaultSpeed, defaultRotationSpeed));
                    else if (type == 5) allNotes.push_back(std::make_unique<NoteType5>(lane, targetTime, defaultSpeed, defaultRotationSpeed));
                }
            } catch (const std::exception& e) {
                continue;
            }
        }
    }
    file.close();

    std::string fullAudioPath = "assets/music_sequence/muzyka1.ogg";
    if (!song.openFromFile(fullAudioPath)) {
        std::cerr << "[BLAD WERYFIKACJI]: Nie udalo sie otworzyc pliku dzwiekowego\n";
        return;
    }

    Conductor::initialize(songBpm, 0.0f);
}

void Game::loadNewSong(const std::string& filename) {
    song.stop();
    if (filename.empty()) return;
    loadSongSequence(filename);
    if (song.getDuration().asMicroseconds() > 0) {
        song.play();
    }
}

void Game::triggerReceptorAnimation(int lane) {
    if (lane >= 0 && lane < 6) {
        receptorScales[lane] = 1.3f;
    }
}

void Game::addFeedback(int lane, const std::string& type, const sf::Color& color) {
    FeedbackText fb;
    fb.text.setFont(font);
    fb.text.setString(type);
    fb.text.setCharacterSize(40);
    fb.text.setFillColor(color);

    float xPos = 50.0f + (lane * 100.0f) - (fb.text.getGlobalBounds().width / 2.0f);
    fb.text.setPosition(xPos, 650.f);
    fb.timer = 0.4f;
    fb.y = 650.f;

    feedbacks.push_back(fb);
}

void Game::update(float deltaTime, float currentTimeMs, InputHandler& inputHandler) {
    if (isPaused) return;

    // Pobieramy czas z Conductora
    Conductor::update(song);
    float audioTimeMs = Conductor::songPosition;

    lastUpdatedTime = audioTimeMs;
    int currentWholeBeat = static_cast<int>(Conductor::currentBeat);

    infoText.setString("BEAT: " + std::to_string(Conductor::currentBeat) + "  |  BPM: " + std::to_string(static_cast<int>(Conductor::bpm)));
    cloudModifier.update(deltaTime);

    // 2. WTACZANIE NUT NA EKRAN (1500 ms przed ich targetTime)
    auto it = allNotes.begin();
    while (it != allNotes.end()) {
        if (audioTimeMs >= static_cast<float>((*it)->getTargetTime()) - 1500.0f) {
            activeNotes.push_back(std::move(*it));
            it = allNotes.erase(it);
        } else {
            ++it;
        }
    }

    // 3. AKTUALIZACJA POZYCJI (Używamy audioTimeMs!)
    for (auto& note : activeNotes) {
        note->updateSynced(audioTimeMs);
    }

    for (auto& note : activeNotes) {
        if (note->getDuration() > 1 && note->getIsBeingHeld()) {
            int lane = note->getLane();
            bool isPlayerHoldingKey = isLaneKeyPressed[lane];
            bool safetyBufferPassed = (audioTimeMs > static_cast<float>(note->getTargetTime()) + 250.0f);

            if (isPlayerHoldingKey || !safetyBufferPassed) {
                if (currentWholeBeat > note->getLastScoredBeat()) {
                    note->setLastScoredBeat(currentWholeBeat);
                    inputHandler.addScore(10);
                    this->triggerReceptorAnimation(lane);
                }
            } else {
                // Gracz puścił klawisz -> Przerywamy cicho trzymanie ogona.
                // Usunięto wywoływanie MISS - nuta po prostu przestanie dawać punkty.
                note->setBeingHeld(false);
            }
        }
    }

    // =================================================================
    // 5. BEZPIECZNE USUWANIE SKOŃCZONYCH NUT
    // =================================================================
    for (auto activeIt = activeNotes.begin(); activeIt != activeNotes.end(); ) {
        float noteEndTime = static_cast<float>((*activeIt)->getTargetTime());

        // Jeśli nuta jest holdem (duration > 1), jej czas życia wydłuża się o czas trwania ogona
        if ((*activeIt)->getDuration() > 1) {
            // Zakładamy długość holda z pliku jako milisekundy (np. 2535ms)
            noteEndTime += static_cast<float>((*activeIt)->getDuration());
        }

        // Warunek usunięcia: Czas audio przekroczył całkowity czas życia nuty
        bool noteIsFinished = (audioTimeMs > noteEndTime);

        if ((*activeIt)->getDuration() > 1 && (*activeIt)->getIsBeingHeld() && !noteIsFinished) {
            // Dopóki hold trwa i nie minął jego pełny czas końcowy - nie usuwamy go!
            ++activeIt;
        }
        else if (noteIsFinished) {
            activeIt = activeNotes.erase(activeIt);
        }
        else {
            ++activeIt;
        }
    }

    // 6. CZYSZCZENIE NUT KTÓRE SPADŁY POD EKRAN I ZOSTAŁY POMINIĘTE
    activeNotes.erase(std::remove_if(activeNotes.begin(), activeNotes.end(),
                                     [](const std::unique_ptr<Note>& note) {
                                         if (note->getDuration() > 1 && note->getIsBeingHeld()) {
                                             return false;
                                         }
                                         return note->getY() > 1050.0f;
                                     }), activeNotes.end());

    // 7. FEEDBACKI TEKSTOWE
    for (auto fbIt = feedbacks.begin(); fbIt != feedbacks.end(); ) {
        fbIt->timer -= deltaTime;
        fbIt->y -= 80.0f * deltaTime;
        fbIt->text.setPosition(fbIt->text.getPosition().x, fbIt->y);
        if (fbIt->timer <= 0.0f) fbIt = feedbacks.erase(fbIt);
        else ++fbIt;
    }
}

// ==========================================================
// TO MUSI BYĆ NA SAMYM DOLE PLIKU Game.cpp (zamiast Note::draw)
// ==========================================================

void Game::draw(sf::RenderWindow& window) {
    float beatProgress = Conductor::currentBeat - std::floor(Conductor::currentBeat);
    sf::Uint8 lineAlpha = static_cast<sf::Uint8>(40.0f + (1.0f - beatProgress) * 60.0f);

    // 1. Rysowanie linii siatki
    for (int i = 1; i <= 5; ++i) {
        float xPos = 50.0f + (i * 100.0f);
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(xPos, 0.f), sf::Color(100, 100, 100, lineAlpha)),
            sf::Vertex(sf::Vector2f(xPos, 1000.f), sf::Color(100, 100, 100, lineAlpha))
        };
        window.draw(line, 2, sf::Lines);
    }

    // 2. Rysowanie receptorów
    for (int i = 1; i <= 5; ++i) {
        float xPos = 50.0f + (i * 100.0f);
        float yPos = 750.0f;

        sf::CircleShape receptor(25.0f);
        receptor.setOrigin(25.0f, 25.0f);
        receptor.setPosition(xPos, yPos);
        receptor.setFillColor(sf::Color(50, 50, 50, 150));

        sf::Color laneColor;
        if (i == 1) laneColor = sf::Color::Green;
        else if (i == 2) laneColor = sf::Color::Red;
        else if (i == 3) laneColor = sf::Color::Yellow;
        else if (i == 4) laneColor = sf::Color::Blue;
        else laneColor = sf::Color::Magenta;

        receptor.setOutlineThickness(2.0f);
        receptor.setOutlineColor(laneColor);
        receptor.setScale(receptorScales[i], receptorScales[i]);
        window.draw(receptor);
    }

    // 3. Rysowanie nut na ekranie
    for (auto& note : activeNotes) {
        note->draw(window);
    }

    // 4. Rysowanie reszty elementów (chmury, napisy, pauza)
    cloudModifier.draw(window);
    window.draw(infoText);

    for (const auto& fb : feedbacks) {
        window.draw(fb.text);
    }

    if (isPaused) {
        window.draw(pauseText);
    }
}

bool Game::isSongFinished() const {
    return song.getStatus() == sf::SoundStream::Stopped;
}

// Funkcja pauzy
void Game::togglePause() {
    isPaused = !isPaused;
    if (isPaused) {
        song.pause(); // SFML wstrzymuje odtwarzanie muzyki
    } else {
        song.play();  // Wznawianie
    }
}
