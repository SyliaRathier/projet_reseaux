#include <QApplication>
#include "GraphWidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    GraphWidget graphWidget;
    graphWidget.setWindowTitle("Simulation de Véhicules");
    graphWidget.resize(600, 400);
    graphWidget.show();

    return app.exec();
}
