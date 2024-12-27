#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "GraphWidget.h"  // Inclure GraphWidget

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Crée une instance de GraphWidget
    GraphWidget *graphWidget = new GraphWidget(this);  // 'this' pour parenté avec MainWindow
    setCentralWidget(graphWidget);  // Ajoute GraphWidget comme widget central
}

MainWindow::~MainWindow() {
    delete ui;
}
