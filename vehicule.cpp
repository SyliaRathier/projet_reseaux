#include "vehicule.h"
#include <QGraphicsScene>
#include <QDebug>

Vehicule::Vehicule(QGraphicsScene *scene, const QPointF &startPos, const QVector<QPointF> &path)
    : QGraphicsEllipseItem(-2.5, -2.5, 5, 5), path(path), currentPointIndex(0), direction(1), speed(1.0) {
    setBrush(Qt::red);
    setPos(startPos);
    scene->addItem(this);

    // Initialiser le timer
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &Vehicule::moveToNextPoint);

    if (!path.isEmpty()) {
        moveTimer->start(100);  // Déplacement toutes les 100 ms
    } else {
        qWarning() << "Le chemin est vide pour ce véhicule.";
    }
}


void Vehicule::startMoving() {
    if (!path.isEmpty()) {
        moveTimer->start();
    }
}

void Vehicule::moveToNextPoint() {
    if (path.isEmpty()) return;

    QPointF currentPos = pos();
    QPointF targetPos = path[currentPointIndex];

    // Calculer le prochain pas
    QPointF nextPos = calculateNextStep(currentPos, targetPos, speed);
    setPos(nextPos);

    // Vérifier si le véhicule a atteint le point cible
    if (nextPos == targetPos) {
        // Inverser la direction si nécessaire
        if (direction == 1 && currentPointIndex == path.size() - 1) {
            direction = -1;
        } else if (direction == -1 && currentPointIndex == 0) {
            direction = 1;
        }

        // Passer au point suivant
        currentPointIndex += direction;
    }
}


QPointF Vehicule::calculateNextStep(const QPointF &currentPos, const QPointF &targetPos, qreal speed) {
    QLineF line(currentPos, targetPos);
    if (line.length() <= speed) {
        return targetPos;
    } else {
        QLineF moveLine(currentPos, targetPos);
        moveLine.setLength(speed);
        return moveLine.p2();
    }
}


// void Vehicule::extendPath(int pointsToAdd) {
//     if (path.isEmpty()) return;

//     // Générateur aléatoire moderne
//     std::random_device rd;
//     std::mt19937 gen(rd());
//     std::uniform_int_distribution<> dis(-50, 50);  // Intervalle aléatoire pour générer des points autour du dernier point

//     QPointF lastPoint = path.last();
//     for (int i = 0; i < pointsToAdd; ++i) {
//         // Calcul des nouveaux points autour du dernier point
//         QPointF newPoint(lastPoint.x() + dis(gen), lastPoint.y() + dis(gen));
//         path.append(newPoint);
//         lastPoint = newPoint;  // Mise à jour du dernier point
//     }
// }
