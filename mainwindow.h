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
    // GUI slots
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_6_clicked();
    void fill_table(QNetworkReply*, int);

    // Network slots
    void response_received(QNetworkReply*);
    void response_master_recieved(QNetworkReply*);
    void updateTable();

    // settings slots
    void submit_dialog_button_clicked();
    void cancel_dialog_button_clicked();

private:
    QSettings *settings;

    Ui::MainWindow *ui;
    QNetworkAccessManager *manager_slave;
    QNetworkAccessManager *manager_master;
    QNetworkRequest request_slave;
    QNetworkRequest request_master;


    QTimer *timer;
    QDialog *settings_gcu;


    // GCU standart vars
    QString address_master = "192.168.1.168";
    QString address_slave = "127.0.0.1";
    QString version_master = "9080"; // port of GCU version 10
    QString version_slave = "9080"; // port of GCU version 11


    // Settings Vars
    QComboBox *version_slave_text;
    QComboBox *version_master_text;
    QLineEdit *address_master_text;
    QLineEdit *address_slave_text;
    QComboBox *info;
    QComboBox *frequency;
    QString freq_int;
    int mode_view = 0;
};
#endif // MAINWINDOW_H
