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

void Game::update(float deltaTime, float currentTimeMs) {
    if (isPaused) return;
    Conductor::update(song);
    float audioTimeMs = Conductor::songPosition;
    lastUpdatedTime = audioTimeMs;

    infoText.setString("BEAT: " + std::to_string(Conductor::currentBeat) + "  |  BPM: " + std::to_string(static_cast<int>(Conductor::bpm)));

    // DODANO: Aktualizacja chmur
    cloudModifier.update(deltaTime);

    for (int i = 1; i <= 5; ++i) {
        if (Conductor::justTriggeredBeat && receptorScales[i] <= 1.0f) {
            receptorScales[i] = 1.1f;
        }
        if (receptorScales[i] > 1.0f) {
            receptorScales[i] -= 3.0f * deltaTime;
            if (receptorScales[i] < 1.0f) receptorScales[i] = 1.0f;
        }
    }

    auto it = allNotes.begin();
    while (it != allNotes.end()) {
        if (audioTimeMs >= static_cast<float>((*it)->getTargetTime()) - 1500.0f) {
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
                                         return note->getY() > 1000.0f;
                                     }), activeNotes.end());

    for (auto fbIt = feedbacks.begin(); fbIt != feedbacks.end(); ) {
        fbIt->timer -= deltaTime;
        fbIt->y -= 80.0f * deltaTime;
        fbIt->text.setPosition(fbIt->text.getPosition().x, fbIt->y);
        if (fbIt->timer <= 0.0f) fbIt = feedbacks.erase(fbIt);
        else ++fbIt;
    }
}

void Game::draw(sf::RenderWindow& window) {
    float beatProgress = Conductor::currentBeat - std::floor(Conductor::currentBeat);
    sf::Uint8 lineAlpha = static_cast<sf::Uint8>(40.0f + (1.0f - beatProgress) * 60.0f);

    for (int i = 1; i <= 5; ++i) {
        float xPos = 50.0f + (i * 100.0f);
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(xPos, 0.f), sf::Color(100, 100, 100, lineAlpha)),
            sf::Vertex(sf::Vector2f(xPos, 1000.f), sf::Color(100, 100, 100, lineAlpha))
        };
        window.draw(line, 2, sf::Lines);
    }

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

    for (auto& note : activeNotes) {
        note->draw(window);
    }

    // DODANO: Rysowanie chmur NAD nutami, ale POD tekstami interfejsu
    cloudModifier.draw(window);

    window.draw(infoText);
    for (const auto& fb : feedbacks) {
        window.draw(fb.text);
    }

    // --- DODANO: Wyświetlanie napisu pauzy na wierzchu ---
    if (isPaused) {
        window.draw(pauseText);
    }
}

bool Game::isSongFinished() const {
    return song.getStatus() == sf::SoundStream::Stopped;
}

//funkcja pauzy
void Game::togglePause() {
    isPaused = !isPaused;
    if (isPaused) {
        song.pause(); // SFML wstrzymuje odtwarzanie muzyki
    } else {
        song.play();  // Wznawianie
    }
}
