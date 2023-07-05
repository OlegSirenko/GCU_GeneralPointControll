#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QStyle>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::gradualChange(const QString &paramName, int targetValue, QPushButton *button)
{
    button->setEnabled(false);
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, timer, paramName, targetValue, button]() {
        static float value = 0;
        QNetworkRequest request(QUrl(QString("http://192.168.1.168:9080/mandala?%1=%2").arg(paramName).arg(value)));
        QNetworkReply *reply = manager->get(request);
        value += 0.00001;
        if (value > targetValue) {
            timer->stop();
            timer->deleteLater();
            value = 0;
            button->setEnabled(true);
            QIcon icon = style()->standardIcon(QStyle::SP_DialogApplyButton);
            button->setIcon(icon);
        }
    });
    timer->start(1);
}


void MainWindow::on_eleron0_clicked()
{
   gradualChange("ctr_ailerons", 1, ui->eleron0);
}


void MainWindow::on_eleron1_clicked()
{
//    QNetworkRequest request(QUrl("http://127.0.0.1:9080/eleron1"));
//    QNetworkReply *reply = manager->get(request);
}


void MainWindow::on_elv0_clicked()
{
    gradualChange("ctr_elevator", 1, ui->elv0);
}


void MainWindow::on_elv1_clicked()
{
//    QNetworkRequest request(QUrl("http://127.0.0.1:9080/elv1"));
//    QNetworkReply *reply = manager->get(request);
}


void MainWindow::on_rudder_clicked()
{
    gradualChange("ctr_rudder", 1, ui->rudder);
}

