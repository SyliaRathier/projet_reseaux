#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QTimer>
#include <QMap>
#include <QPointF>
#include <QJsonArray>
#include <QVector>

class GraphWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    void loadJsonData();
    void createVehicles();
    void updateVehiclePositions();
    void drawGraph();
    QPointF convertToSceneCoordinates(double lon, double lat);

    QGraphicsScene *scene;
    QTimer *timer;
    QMap<qint64, QPointF> nodeCoordinates; // Coordonnées des nœuds
    QList<QGraphicsEllipseItem*> vehicles; // Liste des véhicules
    QMap<QGraphicsEllipseItem*, QVector<QPointF>> vehicleRoutes; // Itinéraires des véhicules
    QMap<QGraphicsEllipseItem*, int> vehicleRouteIndex; // Index de position pour chaque véhicule dans son itinéraire
    QJsonArray elements;
};

#endif // GRAPHWIDGET_H
