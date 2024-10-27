#include <QApplication>
#include "GraphWidget.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    GraphWidget *graphWidget = new GraphWidget();
    graphWidget->show();

    return app.exec();
}
