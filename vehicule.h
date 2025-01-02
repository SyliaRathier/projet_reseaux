#ifndef VEHICULE_H
#define VEHICULE_H

#include <QGraphicsEllipseItem>
#include <QTimer>
#include <QGraphicsScene>
#include <QVector>
#include <QPointF>

class Vehicule : public QObject, public QGraphicsEllipseItem {
    Q_OBJECT

public:
    Vehicule(QGraphicsScene *scene, const QPointF &startPos, const QVector<QPointF> &path);

    void startMoving();  // Démarrer le mouvement du véhicule
    void extendPath(int pointsToAdd);  // Ajouter des points à l'itinéraire

public slots:
    void moveToNextPoint();  // Déplacer le véhicule vers le prochain point

private:
    QVector<QPointF> path;  // Itinéraire du véhicule (une liste de points)
    int currentPointIndex;  // Indice du point courant dans l'itinéraire
    int direction;  // 1 = avancer, -1 = reculer
    QTimer *moveTimer;  // Timer pour déplacer le véhicule
};

#endif // VEHICULE_H
