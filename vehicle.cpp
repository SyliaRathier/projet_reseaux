#include "Vehicle.h"
#include <QBrush>
#include <QColor>
#include <QRandomGenerator>
#include <QDebug>

Vehicle::Vehicle(QPointF startPos, double signalPower, QGraphicsItem *parent)
    : QGraphicsEllipseItem(parent), signalPower(signalPower), currentPosition(startPos) {
    setRect(-10, -10, 20, 20);  // Taille du cercle représentant le véhicule
    setPos(startPos);  // Position initiale du véhicule
    setSignalPower(signalPower);  // Applique la puissance du signal
}

void Vehicle::move() {
    int deltaX = QRandomGenerator::global()->bounded(-1, 2);  // Génère un nombre aléatoire entre -1 et 1
    int deltaY = QRandomGenerator::global()->bounded(-1, 2);

    currentPosition.setX(currentPosition.x() + deltaX);  // Déplacement aléatoire en x
    currentPosition.setY(currentPosition.y() + deltaY);  // Déplacement aléatoire en y
    setPos(currentPosition);  // Applique la nouvelle position

    qDebug() << "Vehicle moved to:" << currentPosition;  // Affiche les nouvelles positions
}

void Vehicle::setSignalPower(double power) {
    signalPower = power;
    QColor color = Qt::blue;
    color.setAlphaF(power);  // Ajuste la transparence selon la puissance
    setBrush(QBrush(color));

    // Affiche la couverture sous forme de cercle autour du véhicule
    setRect(-10 * signalPower, -10 * signalPower, 20 * signalPower, 20 * signalPower);
}
