#include "Note.h"

Note::Note(int lane, int targetTime, int duration, float speed, float rotationSpeed)
    : lane(lane), targetTime(targetTime), duration(duration), yPos(-100.0f),
    speed(speed), rotation(0.0f), rotationSpeed(rotationSpeed) {}

void Note::updateSynced(float currentSongPositionMs) {
    float timeDifference = static_cast<float>(targetTime) - currentSongPositionMs;
    yPos = 850.0f - (timeDifference * 0.5f);
    rotation = currentSongPositionMs * (rotationSpeed / 100.0f);
}

int Note::getLane() const { return lane; }
int Note::getTargetTime() const { return targetTime; }
int Note::getDuration() const { return duration; }
float Note::getY() const { return yPos; }