#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QSettings>
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
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
    void on_settings_btn_clicked();
    void submit_dialog_button_clicked();
    void update_airspeed();
    void response_slave_received(QNetworkReply*);
    void response_master_recieved(QNetworkReply*);
    void read_data(QNetworkReply*, int);
    void ignition();
    void takeoff_mode();
    void thr_cut();
    void brakes_on_off();
    void thr_plus();
    void update_geoposition();
    void on_master_airspeed_valueChanged(int value);

    void on_automatic_takeoff_btn_clicked();

    void on_brakes_btn_clicked();

    void on_slave_airspeed_valueChanged(int value);

    void on_ign_btn_clicked();

    void on_takeoff_btn_clicked();

    void on_thr_plus_clicked();

    void on_wpt_mode_btn_clicked();

private:
    Ui::MainWindow *ui;
    QSettings *settings;

    QNetworkAccessManager *manager_slave;
    QNetworkAccessManager *manager_master;
    QNetworkRequest request_slave;
    QNetworkRequest request_master;

    QTimer *airspeed_ctr_updater;
    QTimer *position_timer;
    QDialog *settings_gcu;


    // GCU standart vars
    QString address_master = "192.168.1.168";
    QString address_slave = "127.0.0.1";
    /// USE 9280 for GCU 11 ver.
    QString version_master = "9080"; // port of GCU version 10
    QString version_slave = "9080"; // port of GCU version 10
    bool takeoff_init = false;
    bool otriv = false;
    QString master_eta;
    QString slave_eta;

    // Settings Vars
    QComboBox *version_slave_text;
    QComboBox *version_master_text;
    QLineEdit *address_master_text;
    QLineEdit *address_slave_text;
    QComboBox *info;
    QComboBox *frequency;
    QString freq_int;
    int mode_view = 0;
    QStringList wp_list;
    QComboBox *app_layout;
    QLabel *settings_saved;
    double position_master_lat, position_master_lon;
    double position_slave_lat,position_slave_lon;
};
#endif // MAINWINDOW_H
