#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <memory>
#include "Note.h"
#include "NoteType.h" // Zawiera definicje NoteType1, NoteType2...
#include "CloudModifier.h"

class Game {
public:
    Game(const std::string& dummy);
    ~Game() = default;

    void loadNewSong(const std::string& filename);
    void loadSongSequence(const std::string& filename);
    void update(float deltaTime, float currentTimeMs);
    void draw(sf::RenderWindow& window);
    bool isSongFinished() const;

    void triggerReceptorAnimation(int lane);
    // Naprawa błędu z pliku InputHandler.cpp (3 parametry)
    void addFeedback(int lane, const std::string& type, const sf::Color& color);
    CloudModifier& getCloudModifier() { return cloudModifier; }

    std::vector<std::unique_ptr<Note>>& getNotes() { return allNotes; }
    std::vector<std::unique_ptr<Note>>& getActiveNotes() { return activeNotes; }

private:
    sf::Music song;
    std::vector<std::unique_ptr<Note>> allNotes;
    std::vector<std::unique_ptr<Note>> activeNotes;
    CloudModifier cloudModifier;

    struct FeedbackText {
        sf::Text text;
        float timer;
        float y;
    };
    std::vector<FeedbackText> feedbacks;

    float lastUpdatedTime;
    float receptorScales[6]; // Indeksy 1-5, więc rozmiar 6 jest bezpieczniejszy
    sf::Font font;
    sf::Text infoText;
};

#endif // GAME_H