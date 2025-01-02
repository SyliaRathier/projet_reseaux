#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMap>
#include <QPointF>
#include <QJsonArray>
#include <QJsonObject>
#include <QWheelEvent>
#include <QVector>
#include "vehicule.h"

class GraphWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);

    // Méthode ajoutée : combineWays
    QVector<QVector<QPointF>> combineWays();

private:
    QGraphicsScene *scene;
    QMap<qint64, QPointF> nodeCoordinates;  // Coordonnées des noeuds
    QJsonArray elements;  // Contient les éléments OSM ou autres données

    QList<Vehicule*> vehicules;  // Liste des véhicules

    QPointF convertToSceneCoordinates(double lon, double lat);  // Convertir les coordonnées GPS en coordonnées de la scène
    void loadOsmData();  // Charger les données OSM
    void drawGraph();  // Dessiner le graphique
    void wheelEvent(QWheelEvent *event) override;  // Gestion du zoom

    void createVehicules();  // Créer les véhicules

};

#endif // GRAPHWIDGET_H
