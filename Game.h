#pragma once
#include <vector>
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

namespace sf { class RenderWindow; }

class Note;

struct HitFeedback {
    std::string text;
    sf::Color color;
    float timer;
    float x;
    float y;
};


class Game {
private:
    std::vector<std::unique_ptr<Note>> activeNotes;
    std::vector<std::unique_ptr<Note>> allNotes;

    std::string currentSongFile;
    int currentScore;
    int comboMultiplier;

    float receptorScales[5] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    float laneHighlightEndTime[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    float lastUpdatedTime = 0.0f;

    sf::Font font;
    std::vector<HitFeedback> feedbacks;

public:
    std::vector<std::unique_ptr<Note>>& getNotes() { return activeNotes; }

    Game(const std::string& songFilename);
    ~Game();

    void loadSongSequence(const std::string& filename);
    void update(float deltaTime, float currentTimeMs);
    void triggerReceptorAnimation(int lane);
    void draw(sf::RenderWindow& window);
    void loadNewSong(const std::string& filename);
    void addFeedback(int lane, const std::string& text, sf::Color color);
};