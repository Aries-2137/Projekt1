#include "Game.h"
#include "Conductor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>

Game::Game(const std::string& dummy) : lastUpdatedTime(0.0f) {
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cerr << "[BLAD GRY]: Nie znaleziono czcionki arial.ttf!\n";
    }
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

    // Bezpieczna ścieżka względna - zadziała na każdym komputerze
    std::string fullPath = "assets/music_sequence/" + filename;

    std::ifstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "[BLAD WERYFIKACJI]: Nie znaleziono pliku mapy (.txt) pod sciezka wzgledna: " << fullPath << std::endl;
        std::cerr << "[WSKAZOWKA]: Upewnij sie, ze katalog roboczy (Working Directory) jest ustawiony na folder glowny projektu, zawierajacy folder 'assets'!\n";
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

                    if (type == 1) {
                        allNotes.push_back(std::make_unique<NoteType1>(lane, targetTime, defaultSpeed, defaultRotationSpeed));
                    }
                    else if (type == 2) {
                        allNotes.push_back(std::make_unique<NoteType2>(lane, targetTime, defaultSpeed, defaultRotationSpeed));
                    }
                    else if (type == 3) {
                        allNotes.push_back(std::make_unique<NoteType3>(lane, targetTime, defaultSpeed, defaultRotationSpeed));
                    }
                    else if (type == 4) {
                        allNotes.push_back(std::make_unique<NoteType4>(lane, targetTime, defaultSpeed, defaultRotationSpeed));
                    }
                    else if (type == 5) {
                        allNotes.push_back(std::make_unique<NoteType5>(lane, targetTime, defaultSpeed, defaultRotationSpeed));
                    }
                }
            } catch (const std::exception& e) {
                continue;
            }
        }
    }
    file.close();

    std::string fullAudioPath = "assets/music_sequence/muzyka1.ogg";

    if (!song.openFromFile(fullAudioPath)) {
        std::cerr << "[BLAD WERYFIKACJI]: Nie udalo sie otworzyc pliku dzwiekowego pod sciezka wzgledna: " << fullAudioPath << std::endl;
        std::cerr << "[WSKAZOWKA]: Upewnij sie, ze plik 'muzyka1.ogg' znajduje sie w: Nowy folder/assets/music_sequence/\n";
        return;
    }

    Conductor::initialize(songBpm, 0.0f);
    std::cout << "[WERYFIKACJA]: Sukces! Wczytano " << filename << " (" << allNotes.size() << " nut, BPM: " << songBpm << ")\n";
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

// Dopasowana implementacja addFeedback przyjmująca lane, tekst oraz kolor
void Game::addFeedback(int lane, const std::string& type, const sf::Color& color) {
    FeedbackText fb;
    fb.text.setFont(font);
    fb.text.setString(type);
    fb.text.setCharacterSize(40);
    fb.text.setFillColor(color);

    // Pozycjonowanie napisu nad receptorem danej ścieżki (zgodnie z getXForLane z NoteType.cpp)
    float xPos = 50.0f + (lane * 100.0f) - (fb.text.getGlobalBounds().width / 2.0f);
    fb.text.setPosition(xPos, 650.f);

    fb.timer = 0.4f;
    fb.y = 650.f;

    feedbacks.push_back(fb);
}

void Game::update(float deltaTime, float currentTimeMs) {
    Conductor::update(song);
    float audioTimeMs = Conductor::songPosition;
    lastUpdatedTime = audioTimeMs;

    infoText.setString("BEAT: " + std::to_string(Conductor::currentBeat) + "  |  BPM: " + std::to_string(static_cast<int>(Conductor::bpm)));

    // Aktualizacja pulsowania receptorów (indeksy 1-5 zgodne z Twoim InputHandlerem)
    for (int i = 1; i <= 5; ++i) {
        // Jeśli Conductor właśnie odpalił nowy beat, receptory lekko pulsują w tle
        if (Conductor::justTriggeredBeat && receptorScales[i] <= 1.0f) {
            receptorScales[i] = 1.1f;
        }

        // Płynny powrót receptorów do podstawowego rozmiaru (1.0f)
        if (receptorScales[i] > 1.0f) {
            receptorScales[i] -= 3.0f * deltaTime; // prędkość zanikania animacji kliknięcia
            if (receptorScales[i] < 1.0f) receptorScales[i] = 1.0f;
        }
    }

    // Przenoszenie nut z allNotes do activeNotes (1500ms przed ich czasem trafienia)
    auto it = allNotes.begin();
    while (it != allNotes.end()) {
        if (audioTimeMs >= static_cast<float>((*it)->getTargetTime()) - 1500.0f) {
            activeNotes.push_back(std::move(*it));
            it = allNotes.erase(it);
        } else {
            ++it;
        }
    }

    // Aktualizacja pozycji aktywnych nut
    for (auto& note : activeNotes) {
        note->updateSynced(audioTimeMs);
    }

    // Usuwanie nut, które spadły poniżej ekranu (MISS)
    activeNotes.erase(std::remove_if(activeNotes.begin(), activeNotes.end(),
                                     [](const std::unique_ptr<Note>& note) {
                                         return note->getY() > 1000.0f;
                                     }), activeNotes.end());

    // Aktualizacja napisów Feedback (PERFECT, GOOD, MISS)
    for (auto fbIt = feedbacks.begin(); fbIt != feedbacks.end(); ) {
        fbIt->timer -= deltaTime;
        fbIt->y -= 80.0f * deltaTime; // Unoszenie napisu do góry
        fbIt->text.setPosition(fbIt->text.getPosition().x, fbIt->y);
        if (fbIt->timer <= 0.0f) fbIt = feedbacks.erase(fbIt);
        else ++fbIt;
    }
}

void Game::draw(sf::RenderWindow& window) {
    // 1. RYSOWANIE PULSUJĄCYCH PIONOWYCH LINII (Siatka dla ścieżek 1-5)
    // Wyliczamy przezroczystość linii na podstawie pozycji w beacie, aby płynnie pulsowały
    float beatProgress = Conductor::currentBeat - std::floor(Conductor::currentBeat);
    sf::Uint8 lineAlpha = static_cast<sf::Uint8>(40.0f + (1.0f - beatProgress) * 60.0f);

    for (int i = 1; i <= 5; ++i) {
        // Środek ścieżki X wyliczony dokładnie tak, jak w Twoim NoteType.cpp
        float xPos = 50.0f + (i * 100.0f);

        // Tworzymy pionową linię od góry do dołu ekranu
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(xPos, 0.f), sf::Color(100, 100, 100, lineAlpha)),
            sf::Vertex(sf::Vector2f(xPos, 1000.f), sf::Color(100, 100, 100, lineAlpha))
        };
        window.draw(line, 2, sf::Lines);
    }

    // 2. RYSOWANIE RECEPTORÓW (Względne miejsce kliknięcia na osi Y = 750.0f)
    for (int i = 1; i <= 5; ++i) {
        float xPos = 50.0f + (i * 100.0f);
        float yPos = 750.0f; // Punkt zero dla Twoich nut z Note::updateSynced

        // Baza receptora (szary, półprzezroczysty okrąg o promieniu zgodnym z nutami = 25px)
        sf::CircleShape receptor(25.0f);
        receptor.setOrigin(25.0f, 25.0f);
        receptor.setPosition(xPos, yPos);
        receptor.setFillColor(sf::Color(50, 50, 50, 150));

        // Obwódka receptora w kolorze odpowiadającym danej ścieżce
        sf::Color laneColor;
        if (i == 1) laneColor = sf::Color::Green;
        else if (i == 2) laneColor = sf::Color::Red;
        else if (i == 3) laneColor = sf::Color::Yellow;
        else if (i == 4) laneColor = sf::Color::Blue;
        else laneColor = sf::Color::Magenta;

        receptor.setOutlineThickness(2.0f);
        receptor.setOutlineColor(laneColor);

        // Zastosowanie skali pulsowania/kliknięcia z tablicy receptorScales
        receptor.setScale(receptorScales[i], receptorScales[i]);

        window.draw(receptor);
    }

    // 3. RYSOWANIE NUT (Muszą być rysowane nad liniami i receptorami!)
    for (auto& note : activeNotes) {
        note->draw(window);
    }

    // 4. RYSOWANIE TEKSTÓW INTERFEJSU I FEEDBACKU
    window.draw(infoText);
    for (const auto& fb : feedbacks) {
        window.draw(fb.text);
    }
}

bool Game::isSongFinished() const {
    return song.getStatus() == sf::SoundStream::Stopped;
}