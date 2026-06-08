#include "Note.h"
#include <iostream>


Note::Note(int lane, int targetTime, int duration, float speed, float rotationSpeed)
    : lane(lane), targetTime(targetTime), duration(duration),
    yPos(-100.0f),
    speed(speed), rotation(0.0f), rotationSpeed(rotationSpeed)
{
}

void Note::update(float deltaTime) {
    // Ruch liniowy w osi Y
    yPos += speed * deltaTime;

    rotation += rotationSpeed * deltaTime;

    if (rotation >= 360.0f) {
        rotation -= 360.0f;
    }
}

int Note::getLane() const { return lane; }
int Note::getTargetTime() const { return targetTime; }
int Note::getDuration() const { return duration; }
float Note::getY() const { return yPos; }