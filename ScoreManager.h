#pragma once
#include <string>
#include <vector>

class ScoreManager {
private:
    std::vector<int> highScores;
    std::string filename;

public:
    // Konstruktor przyjmujący nazwę pliku zapisu (np. "scores.dat")
    ScoreManager(const std::string& file);

    void loadScores();
    void saveScores();

    // Dodanie nowego wyniku na koniec gry
    void addScore(int score);

    // Wyświetlenie tablicy
    void displayScores() const;
};