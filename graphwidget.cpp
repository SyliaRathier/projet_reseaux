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

        // Collect coordinates for this element
        for (const QJsonValue &nodeId : nodes) {
            qint64 id = nodeId.toVariant().toLongLong();
            if (nodeCoordinates.contains(id)) {
                points.append(nodeCoordinates[id]);
            }
        }

        // Skip elements without valid coordinates
        if (points.isEmpty()) {
            continue;
        }

        // Set up the painter for smoother lines
        QPen pen;
        pen.setJoinStyle(Qt::RoundJoin);  // Ensure round joins between lines
        pen.setCapStyle(Qt::RoundCap);    // Smooth line caps

        // Adjust line thickness slightly
        double lineThickness = 1.2;  // Increase the thickness for clearer lines
        pen.setWidthF(lineThickness); // Apply line thickness

        // Create a path item for the roads
        QGraphicsPathItem *pathItem = new QGraphicsPathItem();
        pathItem->setPen(pen);

        // Drawing the map features based on their type
        if (type == "road") {
            // Draw roads with thicker lines
            QPainterPath path(points.first());
            for (int i = 1; i < points.size(); ++i) {
                path.lineTo(points[i]);
            }
            pathItem->setPath(path);
            scene->addItem(pathItem);
        } else if (type == "building") {
            // Draw buildings without borders (no pen)
            QGraphicsPolygonItem *building = new QGraphicsPolygonItem();
            QPolygonF polygon(points);
            building->setPolygon(polygon);
            building->setBrush(QBrush(Qt::lightGray));  // Light gray for buildings
            building->setPen(QPen(Qt::transparent));    // No border
            scene->addItem(building);
        } else if (type == "water") {
            // Draw water bodies (rivers, lakes) in blue with more opacity
            QGraphicsPolygonItem *water = new QGraphicsPolygonItem();
            QPolygonF polygon(points);
            water->setPolygon(polygon);
            water->setBrush(QBrush(Qt::darkBlue));  // Blue for water
            water->setOpacity(0.6);  // Slight transparency for water bodies
            scene->addItem(water);
        } else if (type == "park" || type == "garden") {
            // Handle parks and gardens specifically
            QColor color = (type == "park") ? Qt::green : Qt::darkGreen; // Different colors
            double opacity = (type == "park") ? 0.6 : 0.5;  // Adjust transparency for parks vs gardens

            QGraphicsPolygonItem *item = new QGraphicsPolygonItem(QPolygonF(points));
            item->setBrush(color);
            item->setOpacity(opacity);
            scene->addItem(item);

        } else if (type == "residential") {
            // Draw residential areas in light yellow with slight opacity
            QGraphicsPolygonItem *residential = new QGraphicsPolygonItem();
            QPolygonF polygon(points);
            residential->setPolygon(polygon);
            residential->setBrush(QBrush(Qt::yellow));  // Light yellow for residential
            residential->setOpacity(0.8);  // Slight transparency
            scene->addItem(residential);
        }
    }

    // Adjust the view to show the entire map
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
