#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QNetworkAccessManager>
#include <QMainWindow>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_eleron0_clicked();

    void on_eleron1_clicked();

    void on_elv0_clicked();

    void on_elv1_clicked();

    void on_rudder_clicked();

    void gradualChange(const QString &, int, QPushButton*);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;
};
#endif // MAINWINDOW_H
