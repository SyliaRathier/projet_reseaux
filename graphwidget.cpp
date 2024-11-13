#include "GraphWidget.h"

GraphWidget::GraphWidget(QWidget *parent) : QWidget(parent) {
    // Ajout de noeuds et arêtes comme avant
    addNode(QPointF(100, 100), "Porte Jeune");
    addNode(QPointF(200, 150), "Place de la Réunion");
    addNode(QPointF(300, 100), "Gare Centrale");
    addNode(QPointF(250, 200), "Nouveau Bassin");
    addNode(QPointF(150, 250), "Parc Salvator");

    addEdge(0, 1);
    addEdge(1, 2);
    addEdge(0, 3);
    addEdge(3, 4);
    addEdge(1, 4);

    // Ajouter des voitures entre les noeuds, avec des vitesses différentes
    addCar(0, 1, 0.01); // Voiture entre Porte Jeune et Place de la Réunion
    addCar(1, 2, 0.02); // Voiture entre Place de la Réunion et Gare Centrale
    addCar(3, 4, 0.02); // Voiture entre Parc salvator et nouveau bassin


    // Configurer le timer pour animer les voitures
    connect(&timer, &QTimer::timeout, this, &GraphWidget::updateCars);
    timer.start(50); // Mise à jour toutes les 50 ms
}

void GraphWidget::addNode(const QPointF& pos, const QString& name) {
    nodes.push_back({pos, name});
}

void GraphWidget::addEdge(int startNode, int endNode) {
    edges.push_back({startNode, endNode});
}

void GraphWidget::addCar(int startNode, int endNode, float speed) {
    // Obtenir les positions des noeuds de départ et d'arrivée
    QPointF startPosition = nodes[startNode].position;
    QPointF endPosition = nodes[endNode].position;
    cars.push_back(Car(startPosition, endPosition, speed));
}

void GraphWidget::updateCars() {
    // Met à jour la position de chaque voiture
    for (auto &car : cars) {
        car.updatePosition();
    }
    update(); // Redessine le widget
}

void GraphWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Dessiner les arêtes
    painter.setPen(QPen(Qt::black, 2));
    for (const auto& edge : edges) {
        const QPointF& startPos = nodes[edge.startNode].position;
        const QPointF& endPos = nodes[edge.endNode].position;
        painter.drawLine(startPos, endPos);
    }

    // Dessiner les noeuds
    painter.setBrush(Qt::blue);
    for (const auto& node : nodes) {
        painter.drawEllipse(node.position, 8, 8); // Noeuds en cercles bleus
        painter.drawText(node.position.x() + 10, node.position.y(), node.name); // Nom de l'intersection
    }

    // Dessiner les voitures
    painter.setBrush(Qt::red);
    for (const auto& car : cars) {
        QPointF carPos = car.getPosition();
        painter.drawEllipse(carPos, 5, 5); // Voitures en petits cercles rouges
    }
}
