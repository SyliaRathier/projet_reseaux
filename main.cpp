#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow w;  // Crée la fenêtre principale
    w.show();  // Affiche la fenêtre principale

    return app.exec();  // Démarre l'application
}
