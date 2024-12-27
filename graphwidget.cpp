#include "GraphWidget.h"
#include <QRandomGenerator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QGraphicsLineItem>
#include <QWheelEvent>

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(this)), timer(new QTimer(this)) {
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    setMinimumSize(800, 600);

    loadJsonData();      // Charge les données JSON
    createVehicles();    // Crée les véhicules
    connect(timer, &QTimer::timeout, this, &GraphWidget::updateVehiclePositions);
    timer->start(100);   // Met à jour les positions toutes les 100 ms
}

QPointF GraphWidget::convertToSceneCoordinates(double lon, double lat) {
    const double scaleLon = 10000;
    const double scaleLat = 10000;
    return QPointF(lon * scaleLon, -lat * scaleLat);
}

void GraphWidget::loadJsonData() {
    QFile file(":/data/mulhouse.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Impossible d'ouvrir le fichier JSON";
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qWarning() << "Le document JSON est invalide";
        return;
    }

    QJsonObject jsonObj = doc.object();
    elements = jsonObj["elements"].toArray();

    for (const QJsonValue &value : elements) {
        QJsonObject element = value.toObject();
        if (element["type"] == "node") {
            qint64 id = element["id"].toVariant().toLongLong();
            double lat = element["lat"].toDouble();
            double lon = element["lon"].toDouble();
            nodeCoordinates[id] = convertToSceneCoordinates(lon, lat);
        }
    }

    drawGraph();
}

void GraphWidget::wheelEvent(QWheelEvent *event) {
    const double scaleFactor = 1.15;
    if (event->angleDelta().y() > 0) {
        scale(scaleFactor, scaleFactor);
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void GraphWidget::drawGraph() {
    for (const QJsonValue &value : elements) {
        QJsonObject element = value.toObject();
        if (element["type"] == "way") {
            QJsonArray nodes = element["nodes"].toArray();
            for (int i = 0; i < nodes.size() - 1; ++i) {
                qint64 id1 = nodes[i].toVariant().toLongLong();
                qint64 id2 = nodes[i + 1].toVariant().toLongLong();

                if (nodeCoordinates.contains(id1) && nodeCoordinates.contains(id2)) {
                    QPointF point1 = nodeCoordinates[id1];
                    QPointF point2 = nodeCoordinates[id2];

                    QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(point1, point2));
                    scene->addItem(line);
                }
            }
        }
    }
}

void GraphWidget::createVehicles() {
    int numVehicles = 5;  // Par exemple, créez 5 véhicules

    for (int i = 0; i < numVehicles; ++i) {
        QGraphicsEllipseItem *vehicle = new QGraphicsEllipseItem(-5, -5, 10, 10);
        vehicle->setBrush(Qt::red);
        scene->addItem(vehicle);
        vehicles.append(vehicle);

        // Choisir un nœud de départ pour le véhicule
        auto startNode = nodeCoordinates.begin() + (i % nodeCoordinates.size());
        QVector<QPointF> itinerary;
        itinerary.append(startNode.value());  // Point de départ

        // Crée un itinéraire en suivant les connexions (arêtes) du graphe
        for (const QJsonValue &value : elements) {
            QJsonObject element = value.toObject();
            if (element["type"] == "way") {
                QJsonArray nodes = element["nodes"].toArray();
                for (int j = 0; j < nodes.size(); ++j) {
                    qint64 nodeId = nodes[j].toVariant().toLongLong();
                    if (nodeCoordinates.contains(nodeId)) {
                        itinerary.append(nodeCoordinates[nodeId]);
                    }
                }
            }
        }

        vehicleRoutes[vehicle] = itinerary;
        vehicleRouteIndex[vehicle] = 0;

        // Positionner le véhicule au premier nœud de l'itinéraire
        if (!itinerary.isEmpty()) {
            vehicle->setPos(itinerary.first());
        }
    }
}


void GraphWidget::updateVehiclePositions() {
    const double speed = 2.0;  // Vitesse de déplacement des véhicules

    for (QGraphicsEllipseItem *vehicle : vehicles) {
        QVector<QPointF> route = vehicleRoutes[vehicle];
        int currentIndex = vehicleRouteIndex[vehicle];

        if (currentIndex < route.size() - 1) {
            QPointF currentPos = vehicle->pos();
            QPointF targetPos = route[currentIndex + 1];

            // Calculer la direction du mouvement vers le prochain nœud
            QPointF direction = targetPos - currentPos;
            double distance = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

            if (distance < speed) {
                // Atteint le nœud cible, passe au nœud suivant
                vehicle->setPos(targetPos);
                vehicleRouteIndex[vehicle] += 1;
            } else {
                // Avance vers le prochain nœud
                direction /= distance;  // Normalise la direction
                vehicle->moveBy(direction.x() * speed, direction.y() * speed);
            }
        } else {
            // L'itinéraire est terminé, réinitialisation si nécessaire
            vehicleRouteIndex[vehicle] = 0;  // Recommence depuis le début de l'itinéraire
        }
    }
}
