#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QTimer>
#include <QSettings>
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>

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
    void on_pushButton_clicked();
    void response_received(QNetworkReply*);
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void updateTable();

    void on_pushButton_8_clicked();

    void on_pushButton_6_clicked();

    void submit_dialog_button_clicked();
    void cancel_dialog_button_clicked();

private:
    QSettings *settings;
    int version = 10;
    Ui::MainWindow *ui;
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    QTimer *timer;
    QDialog *settings_gcu;


    QComboBox *version_slave_text;
    QComboBox *version_master_text;
    QLineEdit *address_text;
    QComboBox *info;
    QComboBox *frequency;
};
#endif // MAINWINDOW_H
