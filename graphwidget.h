#include <QGraphicsView>
#include <QMap>
#include <QPointF>
#include <QJsonArray>

class GraphWidget : public QGraphicsView {
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);

private:
    QGraphicsScene *scene;
    QMap<qint64, QPointF> nodeCoordinates;
    QJsonArray elements;  // Ajout de elements ici

    QPointF convertToSceneCoordinates(double lon, double lat);
    void loadJsonData();
    void drawGraph();
    void wheelEvent(QWheelEvent *event) override; // Déclaration de la méthode de zoom

};
