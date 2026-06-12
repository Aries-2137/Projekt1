#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <memory>
#include "Note.h"
#include "NoteType.h"
#include "CloudModifier.h" // <-- DODANO

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
    void addFeedback(int lane, const std::string& type, const sf::Color& color);

    std::vector<std::unique_ptr<Note>>& getNotes() { return allNotes; }
    std::vector<std::unique_ptr<Note>>& getActiveNotes() { return activeNotes; }

    // NOWOŚĆ: Dostęp do modyfikatora chmur
    CloudModifier& getCloudModifier() { return cloudModifier; }

private:
    sf::Music song;
    std::vector<std::unique_ptr<Note>> allNotes;
    std::vector<std::unique_ptr<Note>> activeNotes;

    struct FeedbackText {
        sf::Text text;
        float timer;
        float y;
    };
    std::vector<FeedbackText> feedbacks;

    float lastUpdatedTime;
    float receptorScales[6];
    sf::Font font;
    sf::Text infoText;

    CloudModifier cloudModifier; // <-- DODANO
};

#endif // GAME_H