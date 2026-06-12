#pragma once
#include <string>
#include <vector>

struct ScoreEntry {
    std::string nickname;
    int score;
};

class ScoreManager {
private:
    std::vector<ScoreEntry> highScores;
    std::string filename;

public:
    // Konstruktor przyjmujący nazwę pliku zapisu (np. "scores.dat")
    ScoreManager(const std::string& file);

    void loadScores();
    void saveScores();

    // Dodanie nowego wyniku wraz z pseudonimem na koniec gry
    void addScore(const std::string& nickname, int score);

    // Pobranie listy wyników (potrzebne do rysowania w interfejsie)
    const std::vector<ScoreEntry>& getScores() const { return highScores; }

    // Wyświetlenie tablicy w konsoli
    void displayScores() const;
};