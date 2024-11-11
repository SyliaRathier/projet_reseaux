#include <QApplication>
#include <QWidget>

#include "MainWindow.h"
#include "Hexagon.h"
#include <QVBoxLayout>  // Ajouté pour organiser les widgets

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), numRows(20), numCols(30), radius(40) {
    QWidget *centralWidget = new QWidget(this);  // Crée un widget central pour la fenêtre
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);  // Utilisation d'un layout pour placer les éléments

    graphWidget = new GraphWidget();  // Crée le GraphWidget
    layout->addWidget(graphWidget);  // Ajoute le GraphWidget au layout
    generateHexagons();
      // Génère les hexagones
    resize(1200, 800);  // Taille de la fenêtre
}

MainWindow::~MainWindow() {}

void MainWindow::generateHexagons() {
    float hexHeight = sqrt(3) * radius;  // Calcule la hauteur des hexagones

    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            // Calcule la position x et y des hexagones
            float x = col * (1.5 * radius);  // Espacement entre les colonnes
            float y = row * hexHeight;  // Position verticale de base
            if (col % 2 != 0) {
                y += hexHeight / 2;  // Décalage pour les colonnes impaires
            }
            hexagons.push_back(Hexagon(x, y, radius));  // Crée chaque hexagone
        }
    }
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    for (const auto &hex : hexagons) {
        hex.draw(painter);  // Dessine chaque hexagone
    }
}
