#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include <vector>
#include "Car.h" // Inclure la classe Car

struct Node {
    QPointF position;
    QString name; // Nom de l'intersection
};

struct Edge {
    int startNode;
    int endNode;
};

class GraphWidget : public QWidget {
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);
    void addNode(const QPointF& pos, const QString& name);
    void addEdge(int startNode, int endNode);
    void addCar(int startNode, int endNode, float speed); // Ajoute une voiture

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateCars(); // Mise à jour des positions de voitures

private:
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    std::vector<Car> cars;
    QTimer timer;
};

#endif // GRAPHWIDGET_H
