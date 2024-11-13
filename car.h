#ifndef CAR_H
#define CAR_H

#include <QPointF>

class Car {
public:
    Car(QPointF startPosition, QPointF endPosition, float speed);

    void updatePosition(); // Met à jour la position de la voiture
    QPointF getPosition() const; // Récupère la position actuelle de la voiture

private:
    QPointF startPosition;
    QPointF endPosition;
    float speed;     // Vitesse de déplacement
    float progress;  // Entre 0 et 1 pour représenter la position entre startPosition et endPosition
};

#endif // CAR_H
