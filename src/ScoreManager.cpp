#include "ScoreManager.h"
#include <fstream>
#include <iostream>
#include <algorithm>

ScoreManager::ScoreManager(const std::string& file) : filename(file) {
    loadScores();
}

void ScoreManager::loadScores() {
    highScores.clear();
    std::ifstream file(filename);

    if (file.is_open()) {
        int score;
        while (file >> score) {
            highScores.push_back(score);
        }
        file.close();
    }
}

void ScoreManager::saveScores() {
    std::ofstream file(filename);

    if (file.is_open()) {
        for (int score : highScores) {
            file << score << "\n";
        }
        file.close();
    }
}

void ScoreManager::addScore(int score) {
    highScores.push_back(score);
    std::sort(highScores.begin(), highScores.end(), std::greater<int>());
    if (highScores.size() > 10) {
        highScores.resize(10);
    }
    saveScores();
}

void ScoreManager::displayScores() const {
    std::cout << "--- TABLICA WYNIKOW ---\n";
    for (size_t i = 0; i < highScores.size(); ++i) {
        std::cout << i + 1 << ". " << highScores[i] << " pkt\n";
    }
}