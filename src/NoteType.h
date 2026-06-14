#pragma once
#include "Note.h"

class NoteType1 : public Note {
public:
    NoteType1(int lane, int targetTime, float speed, float rotationSpeed);
    void draw(sf::RenderWindow& window) const override;
};

class NoteType2 : public Note {
public:
    NoteType2(int lane, int targetTime, float speed, float rotationSpeed);
    void draw(sf::RenderWindow& window) const override;
};

class NoteType3 : public Note {
public:
    NoteType3(int lane, int targetTime, float speed, float rotationSpeed);
    void draw(sf::RenderWindow& window) const override;
};

class NoteType4 : public Note {
public:
    NoteType4(int lane, int targetTime, float speed, float rotationSpeed);
    void draw(sf::RenderWindow& window) const override;
};

class NoteType5 : public Note {
public:
    NoteType5(int lane, int targetTime, float speed, float rotationSpeed);
    void draw(sf::RenderWindow& window) const override;
};