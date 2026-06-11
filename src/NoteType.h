#pragma once
#include "Note.h"

// --- RODZAJE NUTEK ---
// Typ 1: Długość 1 (samo koło, brak kreski)
class NoteType1 : public Note {
public:
    NoteType1(int lane, int targetTime, float speed, float rotationSpeed);
    void draw(sf::RenderWindow& window) const override;
};

// Typ 2: Długość 2 (koło + krótka kreska)
class NoteType2 : public Note {
public:
    NoteType2(int lane, int targetTime, float speed, float rotationSpeed);
    void draw(sf::RenderWindow& window) const override;
};

// Typ 3: Długość 3 (koło + średnia kreska)
class NoteType3 : public Note {
public:
    NoteType3(int lane, int targetTime, float speed, float rotationSpeed);
    void draw(sf::RenderWindow& window) const override;
};

// Typ 4: Długość 4 (koło + długa kreska)
class NoteType4 : public Note {
public:
    NoteType4(int lane, int targetTime, float speed, float rotationSpeed);
    void draw(sf::RenderWindow& window) const override;
};

// Typ 5: Długość 5 (koło + najdłuższa kreska)
class NoteType5 : public Note {
public:
    NoteType5(int lane, int targetTime, float speed, float rotationSpeed);
    void draw(sf::RenderWindow& window) const override;
};