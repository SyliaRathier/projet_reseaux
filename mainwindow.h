#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include "hexagon.h"
#include "GraphWidget.h"  // Ajout de l'inclusion de GraphWidget

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void generateHexagons();  // Déclaration de la fonction pour générer les hexagones
    std::vector<Hexagon> hexagons;  // Vecteur pour stocker les hexagones
    int numRows;  // Nombre de lignes d'hexagones
    int numCols;  // Nombre de colonnes d'hexagones
    float radius;  // Rayon des hexagones

    GraphWidget *graphWidget;  // Ajout de l'instance de GraphWidget pour le graphe
};

#endif // MAINWINDOW_H
