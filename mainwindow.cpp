#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionOrthographic, &QAction::triggered, [this]()
    {
        ui->widget->setCameraMode(GlCam::Orthographic);
    });
    connect(ui->actionPerspective, &QAction::triggered, [this]()
    {
        ui->widget->setCameraMode(GlCam::Perspective);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

