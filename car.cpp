#include "Car.h"

Car::Car(QPointF startPosition, QPointF endPosition, float speed)
    : startPosition(startPosition), endPosition(endPosition), speed(speed), progress(0.0) {
}

void Car::updatePosition() {
    // Incrémente le progrès en fonction de la vitesse
    progress += speed;
    if (progress > 1.0) {
        progress = 1.0; // Limite le progrès à 1 (fin de l'arête)
    }
}

QPointF Car::getPosition() const {
    // Calcule la position actuelle entre startPosition et endPosition
    return (1 - progress) * startPosition + progress * endPosition;
}
