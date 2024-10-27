#include "GraphWidget.h"
#include <QDebug>
#include <QtConcurrent>
#include <QFuture>
#include <QGraphicsLineItem>
#include <QJsonArray>
#include <QJsonObject>
#include <QWheelEvent>


GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(this)) {
    setScene(scene);
    setRenderHint(QPainter::Antialiasing);
    setMinimumSize(800, 600);

    loadJsonData();  // Charge les données JSON
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
    elements = jsonObj["elements"].toArray();  // Stocke les éléments pour drawGraph

    // Charge les coordonnées des nœuds
    for (const QJsonValue &value : elements) {
        QJsonObject element = value.toObject();
        if (element["type"] == "node") {
            qint64 id = element["id"].toVariant().toLongLong();
            double lat = element["lat"].toDouble();
            double lon = element["lon"].toDouble();
            nodeCoordinates[id] = convertToSceneCoordinates(lon, lat);
        }
    }

    drawGraph();  // Dessine le graphe après le chargement
}

void GraphWidget::wheelEvent(QWheelEvent *event) {
    const double scaleFactor = 1.15;  // Facteur de zoom

    if (event->angleDelta().y() > 0) {
        // Zoom avant
        scale(scaleFactor, scaleFactor);
    } else {
        // Zoom arrière
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

                    // Créer et ajouter une ligne entre les deux points
                    QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(point1, point2));
                    scene->addItem(line);
                }
            }
        }
    }

    // Ajuste la vue pour montrer tout le graphe
    scene->setSceneRect(scene->itemsBoundingRect());  // Définit les limites de la scène
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);  // Ajuste la vue pour le graphe
}
