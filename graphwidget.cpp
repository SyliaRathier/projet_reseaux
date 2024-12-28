#include "GraphWidget.h"
#include <QDebug>
#include <QGraphicsLineItem>
#include <QJsonArray>
#include <QJsonObject>
#include <QWheelEvent>
#include <QGraphicsOpacityEffect>
#include <QPainterPath>
#include <QFile>
#include <QXmlStreamReader>

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(this)) {
    setScene(scene);
    setBackgroundBrush(Qt::white);
    setMinimumSize(1500, 800);
    setMaximumSize(1500, 800); // Keep or remove as needed

    loadOsmData();  // Load and process the OSM data
}

QPointF GraphWidget::convertToSceneCoordinates(double lon, double lat) {
    const double scaleLon = 10000;  // Adjust if necessary
    const double scaleLat = 10000;  // Adjust if necessary

    qDebug() << "Converted coordinates:" << lon << lat
             << "->" << lon * scaleLon << "," << -lat * scaleLat;

    return QPointF(lon * scaleLon, -lat * scaleLat);
}

void GraphWidget::loadOsmData() {
    QFile file(":/data/map.osm"); // **MAKE SURE THIS PATH IS CORRECT**
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open OSM file";
        return;
    }

    QXmlStreamReader xml(&file);
    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();

        if (xml.isStartElement() && xml.name() == "node") {
            qint64 id = xml.attributes().value("id").toLongLong();
            double lat = xml.attributes().value("lat").toDouble();
            double lon = xml.attributes().value("lon").toDouble();
            nodeCoordinates[id] = convertToSceneCoordinates(lon, lat);
        }

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
                    } else if (key == "waterway" || value == "water" || key == "natural") {
                        type = "water";
                    } else if (key == "landuse" && value == "park") {
                        type = "park";
                    } else if (key == "leisure" && value == "garden") {  // Directly handle garden
                        type = "garden";
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
        qWarning() << "Error parsing OSM file:" << xml.errorString();
    }
    file.close();

    drawGraph();  // Draw the map with the parsed data
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
