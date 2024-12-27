#ifndef VEHICLE_H
#define VEHICLE_H

#include <QGraphicsEllipseItem>
#include <QPointF>

class Vehicle : public QGraphicsEllipseItem {
public:
    Vehicle(QPointF startPos, double signalPower, QGraphicsItem *parent = nullptr);
    void move();  // Déplace le véhicule
    void setSignalPower(double power);

private:
    double signalPower;
    QPointF currentPosition;
};

#endif // VEHICLE_H
