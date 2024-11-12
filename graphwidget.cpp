#include "GraphWidget.h"
#include <QDebug>
#include <QtConcurrent>
#include <QFuture>
#include <QGraphicsLineItem>
#include <QJsonArray>
#include <QJsonObject>
#include <QWheelEvent>
#include <QQuickWidget>
#include <QQmlContext>


#include <QGraphicsOpacityEffect>

GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent), scene(new QGraphicsScene(this)) {

    // Set up the QGraphicsView scene
    setScene(scene);
    setBackgroundBrush(Qt::white);
    setMinimumSize(1500, 800);
    setMaximumSize(1500, 800);  // You can remove this line if you want to allow resizing

    // Load the JSON data for the graph
    loadJsonData();

    // Set up the QQuickWidget for the map
    QQuickWidget *mapWidget = new QQuickWidget(this);
    mapWidget->setSource(QUrl(QStringLiteral("qrc:/mapview.qml")));

    // Make the map widget's background transparent
    mapWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    mapWidget->setAttribute(Qt::WA_TranslucentBackground);
    mapWidget->setWindowFlag(Qt::FramelessWindowHint);  // Remove window frame if needed

    // Make the map widget non-interactive so it doesn't block the graph
    mapWidget->setAttribute(Qt::WA_TransparentForMouseEvents);

    // Set the opacity effect to make the map widget semi-transparent
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(mapWidget);
    opacityEffect->setOpacity(0.7);  // Set the opacity (0.0 is fully transparent, 1.0 is fully opaque)
    mapWidget->setGraphicsEffect(opacityEffect);

    // Position mapWidget to cover the entire view area
    mapWidget->setGeometry(0, 0, width(), height());  // Same dimensions as the GraphWidget

    // Ensure the map and graph widgets are layered correctly
    mapWidget->raise();  // Ensure the map is above the graph if needed
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
