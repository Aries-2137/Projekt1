#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include <string>
#include "Note.h"

struct Feedback {
    std::string text;
    sf::Color color;
    float timer;
    float x;
    float y;
};

class Game {
public:
    Game(const std::string& songFilename);
    ~Game();

    void update(float deltaTime, float currentTimeMs);
    void draw(sf::RenderWindow& window);

    void loadSongSequence(const std::string& filename);
    void loadNewSong(const std::string& filename);

    void triggerReceptorAnimation(int lane);
    void addFeedback(int lane, const std::string& text, sf::Color color);

    std::vector<std::unique_ptr<Note>>& getNotes() { return activeNotes; }

    // Poprawiona logika sprawdzania stanu
    bool isSongFinished() const { return hasStarted && song.getStatus() == sf::SoundStream::Stopped; }

private:
    std::string currentSongFile;
    sf::Font font;
    sf::Music song;
    bool hasStarted = false; // Flaga zabezpieczająca

    std::vector<std::unique_ptr<Note>> allNotes;
    std::vector<std::unique_ptr<Note>> activeNotes;
    std::vector<Feedback> feedbacks;

    float receptorScales[5];
    float laneHighlightEndTime[5];
    float lastUpdatedTime;
};

#endif // GAME_H