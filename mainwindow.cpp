#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Créer le widget pour le graphe et le mettre comme widget central
    graphWidget = new GraphWidget(this);
    setCentralWidget(graphWidget);
}

MainWindow::~MainWindow() {
    delete ui;
}
