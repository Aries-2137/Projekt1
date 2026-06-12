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
        std::string nick;
        int score;
        // Odczyt ze struktury pliku tekstowego: NICK WYNIK
        while (file >> nick >> score) {
            highScores.push_back({nick, score});
        }
        file.close();
    }
}

void ScoreManager::saveScores() {
    std::ofstream file(filename);

    if (file.is_open()) {
        for (const auto& entry : highScores) {
            file << entry.nickname << " " << entry.score << "\n";
        }
        file.close();
    }
}

void ScoreManager::addScore(const std::string& nickname, int score) {
    highScores.push_back({nickname, score});

    // Sortowanie malejące (od największego do najmniejszego)
    std::sort(highScores.begin(), highScores.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score > b.score;
    });

    // Ograniczenie tablicy do TOP 10 wyników
    if (highScores.size() > 10) {
        highScores.resize(10);
    }
    saveScores();
}

void ScoreManager::displayScores() const {
    std::cout << "--- TABLICA WYNIKOW ---\n";
    for (size_t i = 0; i < highScores.size(); ++i) {
        std::cout << i + 1 << ". " << highScores[i].nickname << " - " << highScores[i].score << " pkt\n";
    }
}