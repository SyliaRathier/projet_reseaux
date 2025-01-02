#include "GraphWidget.h"
#include <QDebug>
#include <QtConcurrent>
#include <QFuture>
#include <QGraphicsLineItem>
#include <QXmlStreamReader>
#include <QFile>
#include <QWheelEvent>
#include <QGraphicsPolygonItem>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include "vehicule.h"
#include <QJsonArray>

// Constructeur de GraphWidget
GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(this)) {
    setScene(scene);
    setBackgroundBrush(Qt::white);  // Arrière-plan blanc
    setMinimumSize(1500, 800);      // Taille minimale de la fenêtre
    loadOsmData();                  // Charger les données OSM
    createVehicules();              // Créer les véhicules après le chargement
}

// Convertir les coordonnées longitude/latitude en coordonnées de la scène
QPointF GraphWidget::convertToSceneCoordinates(double lon, double lat) {
    const double scaleLon = 10000;  // Facteur d'échelle longitude
    const double scaleLat = 10000;  // Facteur d'échelle latitude

    return QPointF(lon * scaleLon, -lat * scaleLat);  // Inverser la latitude
}

// Charger et analyser les données OSM
void GraphWidget::loadOsmData() {
    QFile file(":/data/map.osm");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Impossible d'ouvrir le fichier OSM";
        return;
    }

    QXmlStreamReader xml(&file);
    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();

        // Analyse des nœuds (nodes)
        if (xml.isStartElement() && xml.name() == "node") {
            qint64 id = xml.attributes().value("id").toLongLong();
            double lat = xml.attributes().value("lat").toDouble();
            double lon = xml.attributes().value("lon").toDouble();
            nodeCoordinates[id] = convertToSceneCoordinates(lon, lat);
        }

        // Analyse des chemins (ways)
        if (xml.isStartElement() && xml.name() == "way") {
            QJsonArray nodes;
            QString type;

            while (!(xml.isEndElement() && xml.name() == "way")) {
                xml.readNext();
                if (xml.isStartElement() && xml.name() == "nd") {
                    qint64 ref = xml.attributes().value("ref").toLongLong();
                    nodes.append(ref);
                } else if (xml.isStartElement() && xml.name() == "tag") {
                    QString key = xml.attributes().value("k").toString();
                    QString value = xml.attributes().value("v").toString();
                    if (key == "highway") {
                        type = "road";
                    } else if (key == "building") {
                        type = "building";
                    } else if (key == "landuse" && value == "residential") {
                        type = "residential";
                    } else if (key == "waterway" || value == "water") {
                        type = "water";
                    } else if (key == "landuse" && value == "park") {
                        type = "park";
                    }
                }
            }

            QJsonObject wayObject;
            wayObject["type"] = type;
            wayObject["nodes"] = nodes;
            elements.append(wayObject);
        }
    }

    if (xml.hasError()) {
        qWarning() << "Erreur lors de l'analyse du fichier OSM :" << xml.errorString();
    }
    file.close();

    drawGraph();  // Dessiner le graphe
}

// Combiner les chemins connectés pour créer des itinéraires plus longs
QVector<QVector<QPointF>> GraphWidget::combineWays() {
    QVector<QVector<QPointF>> combinedPaths;

    QSet<qint64> visitedNodes;
    for (const QJsonValue &value : elements) {
        QJsonObject element = value.toObject();
        if (element["type"].toString() == "road") {
            QVector<QPointF> path;
            QJsonArray nodes = element["nodes"].toArray();

            for (const QJsonValue &nodeId : nodes) {
                qint64 id = nodeId.toVariant().toLongLong();
                if (nodeCoordinates.contains(id) && !visitedNodes.contains(id)) {
                    path.append(nodeCoordinates[id]);
                    visitedNodes.insert(id);
                }
            }

            if (!path.isEmpty()) {
                combinedPaths.append(path);
            }
        }
    }

    return combinedPaths;
}

// Gérer le zoom avec la molette de la souris
void GraphWidget::wheelEvent(QWheelEvent *event) {
    const double scaleFactor = 1.15;

    if (event->angleDelta().y() > 0) {
        scale(scaleFactor, scaleFactor);
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

// Dessiner le graphe/carte à partir des données OSM
void GraphWidget::drawGraph() {
    for (const QJsonValue &value : elements) {
        QJsonObject element = value.toObject();
        QString type = element["type"].toString();
        QJsonArray nodes = element["nodes"].toArray();

        QVector<QPointF> points;

        // Récupérer les coordonnées des nœuds
        for (const QJsonValue &nodeId : nodes) {
            qint64 id = nodeId.toVariant().toLongLong();
            if (nodeCoordinates.contains(id)) {
                points.append(nodeCoordinates[id]);
            }
        }

        if (points.isEmpty()) {
            continue;
        }

        QPen pen;
        pen.setJoinStyle(Qt::RoundJoin);
        pen.setCapStyle(Qt::RoundCap);
        pen.setWidthF(0.5);

        QGraphicsPathItem *pathItem = new QGraphicsPathItem();
        pathItem->setPen(pen);

        if (type == "road") {
            QPainterPath path(points.first());
            for (int i = 1; i < points.size(); ++i) {
                path.lineTo(points[i]);
            }
            pathItem->setPath(path);
            scene->addItem(pathItem);
        } else if (type == "building") {
            QGraphicsPolygonItem *building = new QGraphicsPolygonItem();
            building->setPolygon(QPolygonF(points));
            building->setBrush(QBrush(Qt::lightGray));
            scene->addItem(building);
        }
    }

    scene->setSceneRect(scene->itemsBoundingRect());
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

// Créer des véhicules sur les chemins combinés
void GraphWidget::createVehicules() {
    QVector<QVector<QPointF>> combinedPaths = combineWays();

    int vehiculeCount = 0;
    for (const QVector<QPointF> &path : combinedPaths) {
        if (path.size() >= 2 && vehiculeCount < 20) {
            Vehicule *vehicule = new Vehicule(scene, path.first(), path);
            vehicules.append(vehicule);
            vehicule->startMoving();
            vehiculeCount++;
        } else {
            qWarning() << "Chemin trop court ou limite de véhicules atteinte.";
        }
    }
}
