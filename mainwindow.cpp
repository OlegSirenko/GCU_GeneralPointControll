#include "mainwindow.h"
#include <QXmlStreamReader>
#include <QThread>
#include "./ui_mainwindow.h"


double distance(double lat1, double lon1, double lat2, double lon2) {
    const double r = 6371; // радиус Земли в километрах
    const double pi = 3.14159265358979323846;
    lat1 *= pi / 180; // перевод в радианы
    lon1 *= pi / 180;
    lat2 *= pi / 180;
    lon2 *= pi / 180;
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;
    double a = sin(dlat / 2) * sin(dlat / 2) + cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double d = r * c;
    return d;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    position_master_lat = 0.0;
    position_master_lon = 0.0;
    position_slave_lat=0.0;
    position_slave_lon=0.0;

    // import settings
    settings = new QSettings("gcu_settings");
    settings->beginGroup("Settings");
    address_master = settings->value("Settings/address_master").toString();
    address_slave = settings->value("Settings/address_slave").toString();
    version_master = settings->value("Settings/version_master").toString();
    version_slave = settings->value("Settings/version_slave").toString();
    freq_int = settings->value("Settings/timer_frequency").toString();

    airspeed_ctr_updater = new QTimer; //update
    airspeed_ctr_updater->setInterval(freq_int.toInt());
    connect(airspeed_ctr_updater, SIGNAL(timeout()), this, SLOT(update_airspeed()));
    airspeed_ctr_updater->start();

    position_timer = new QTimer;
    position_timer->setInterval(freq_int.toInt());
    connect(position_timer,SIGNAL(timeout()), this, SLOT(update_geoposition()));
    position_timer->start();
    manager_master = new QNetworkAccessManager();
    manager_slave = new QNetworkAccessManager();
    connect(manager_master, SIGNAL(finished(QNetworkReply*)), this, SLOT(response_master_recieved(QNetworkReply*)));
    connect(manager_slave, SIGNAL(finished(QNetworkReply*)), this, SLOT(response_slave_received(QNetworkReply*)));

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::update_geoposition(){
    request_slave.setUrl(QUrl("http://"+address_slave+":" + version_slave + "/mandala?est.pos.lon&est.pos.lat"));
    manager_slave->get(request_slave);
    request_master.setUrl(QUrl("http://"+address_master+":" + version_master + "/mandala?est.pos.lon&est.pos.lat"));
    manager_master->get(request_master);
    int dist = distance(position_master_lat, position_master_lon, position_slave_lat, position_slave_lon) * 1000;
    ui->distance_doube->setText(QString::number(distance(position_master_lat, position_master_lon, position_slave_lat, position_slave_lon)*1000));
    int master_airspeed_local = ui->slave_airspeed->value() + (dist)*0.3;
    ui->master_airspeed->setValue(master_airspeed_local);
}

void MainWindow::response_master_recieved(QNetworkReply* reply){
    read_data(reply, 0);
}


void MainWindow::response_slave_received(QNetworkReply* reply){
    read_data(reply, 1);
}

void MainWindow::read_data(QNetworkReply*reply, int id){
    QLabel *status_connection;
    status_connection= id == 0 ? ui->status_master_lbl: ui->status_slave_lbl;

    if (reply->error()) {
        qDebug() << " from GCU "<<id<<reply->errorString();
        return;
    }

    status_connection->setText("Подключение установлено");

    QXmlStreamReader *xmlReader;
    xmlReader = new QXmlStreamReader(reply);
    int row_iter = 0;
    QString command_flyTo_x;
    QString command_flyTo_y;
    QString slave_est_pos_lat;
    QString slave_est_pos_lon;
    int wp_number;

    if (id==0){
        while(!xmlReader->atEnd() && !xmlReader->hasError()) {
            QXmlStreamReader::TokenType token = xmlReader->readNext();
            if(token == QXmlStreamReader::StartDocument) continue;
            if(token == QXmlStreamReader::StartElement) {
                if (xmlReader->name() == QString("mandala"))continue;

                QString element_text = xmlReader->readElementText();
                if (xmlReader->name().toString() == "est.pos.lon"){
                    ui->master_lon->setText(element_text);
                    position_master_lon=element_text.toDouble();
                }
                if (xmlReader->name().toString() == "est.pos.lat"){
                    ui->master_lat->setText(element_text);
                    position_master_lat=element_text.toDouble();
                }
            }
        }
    }
    else{
        while(!xmlReader->atEnd() && !xmlReader->hasError()) {
            QXmlStreamReader::TokenType token = xmlReader->readNext();
            if(token == QXmlStreamReader::StartDocument) continue;
            if(token == QXmlStreamReader::StartElement) {
                if (xmlReader->name() == QString("mandala")) continue;
                QString element_text = xmlReader->readElementText();
                if (xmlReader->name().toString() == "est.pos.lon"){
                    ui->slave_lon->setText(element_text);
                    position_slave_lon = element_text.toDouble();
                }
                if (xmlReader->name().toString() == "est.pos.lat"){
                    ui->slave_lat->setText(element_text);
                    position_slave_lat = element_text.toDouble();
                }
            }
        }
    }
}



void MainWindow::update_airspeed(){ // Обновление команды скорост
    int set_master_speed = ui->master_airspeed->value();
    int set_slave_speed = ui->slave_airspeed->value();

    request_slave.setUrl(QUrl("http://"+address_slave+":" + version_slave + "/mandala?cmd.pos.airspeed="+QString::number(set_slave_speed)));
    manager_slave->get(request_slave);

    request_master.setUrl(QUrl("http://"+address_master+":" + version_master + "/mandala?cmd.pos.airspeed="+QString::number(set_master_speed)));
    manager_master->get(request_master);
}

void MainWindow::on_settings_btn_clicked() // Вызов окна настроек
{
    settings_gcu = new QDialog;
    QGridLayout *layout = new QGridLayout;
    QPushButton *submit = new QPushButton;
    QPushButton *cancel = new QPushButton;

    app_layout = new QComboBox;

    app_layout->addItems({"Компактный режим", "Полный режим"});
    qDebug()<<mode_view;
    app_layout->setCurrentIndex(mode_view);

    QLabel *version_slave_label = new QLabel;
    version_slave_text = new QComboBox;
    version_slave_label->setText("Версия ведомого GCU");
    version_slave_text->addItems(QStringList{"10", "11"});

    version_master_text = new QComboBox;
    QLabel *version_master_label = new QLabel;
    version_master_label->setText("Версия ведущего GCU");
    version_master_text->addItems(QStringList{"10", "11"});

    version_slave_text->setCurrentIndex(version_slave=="9080"? 0 : 1);
    version_master_text->setCurrentIndex(version_master=="9080" ? 0: 1);

    QLabel *address = new QLabel;
    QLabel *address_slave_label = new QLabel;
    address_master_text = new QLineEdit;
    address_slave_text = new QLineEdit;

    address->setText("Адрес ведущего GCU");
    address_master_text->setText(address_master);
    address_slave_label->setText("Адрес ведомого GCU");
    address_slave_text->setText(address_slave);

    QLabel *timer_freq = new QLabel;
    frequency = new QComboBox;
    timer_freq->setText("Частота обновления информации");
    frequency->addItems(QStringList{"300", "500", "1000", "1500", "2000", "2500"});
    frequency->setCurrentText(freq_int);


    settings_gcu->setModal(true);
    settings_gcu->setWindowTitle("GCU settings");

    connect(submit, SIGNAL(clicked()), this, SLOT(submit_dialog_button_clicked()));

    settings_gcu->setLayout(layout);

    submit->setText("Сохранить") ;
    cancel->setText("Отмена");

    settings_saved = new QLabel;

    layout->addWidget(version_slave_label, 2, 1);
    layout->addWidget(version_slave_text, 2, 2);
    layout->addWidget(version_master_label, 1, 1);
    layout->addWidget(version_master_text, 1, 2);
    layout->addWidget(address, 3, 1);
    layout->addWidget(address_master_text, 3, 2);
    layout->addWidget(address_slave_label, 4, 1);
    layout->addWidget(address_slave_text, 4, 2);
    layout->addWidget(timer_freq, 5, 1);
    layout->addWidget(frequency, 5, 2);
    //layout->addWidget(app_layout, 6, 1);
    layout->addWidget(submit, 10, 1);
    layout->addWidget(settings_saved, 10, 2);
    settings_gcu->show();
}

void MainWindow::submit_dialog_button_clicked(){  // Сохрание и применение настроек
    if (address_master_text->text().isEmpty()){
        address_master_text->setStyleSheet("QLineEdit { background-color: red }");
        return;
    }
    if (address_slave_text->text().isEmpty()){
        address_slave_text->setStyleSheet("QLineEdit { background-color: red }");
        return;
    }

    version_master = version_master_text->currentText().toInt() == 10? "9080" : "9280";
    version_slave = version_slave_text->currentText().toInt() == 10? "9080": "9280";

    address_master = address_master_text->text();
    address_slave = address_slave_text->text();

    airspeed_ctr_updater->stop();
    airspeed_ctr_updater->setInterval(frequency->currentText().toInt());
    airspeed_ctr_updater->start();

    settings->beginGroup("Settings");
    settings->setValue("address_master", address_master);
    settings->setValue("address_slave", address_slave);
    settings->setValue("version_master", version_master);
    settings->setValue("version_slave", version_slave);
    settings->setValue("timer_frequency", frequency->currentText().toInt());
    settings->setValue("View_mode", app_layout->currentIndex());
    settings->endGroup();
    qDebug() << settings->fileName();
    qDebug()<<"Saved";
    settings_saved->setText("Настройки сохранены");
}


void MainWindow::on_master_airspeed_valueChanged(int value)
{
    ui->airspeed_master_lbl->setText(QString::number(value));
}


void MainWindow::on_automatic_takeoff_btn_clicked()
{
    ignition();
    QThread::msleep(200);
    takeoff_mode();
    QThread::msleep(200);
    brakes_on_off();
    QThread::msleep(350);
    thr_plus();
    QThread::msleep(200);
}

void MainWindow::ignition(){
    qDebug()<<"Зажигание";
    QString command_slave = version_slave == "9080" ? "src=btn_IGN()" : "scr=btn_ENG()";
    QString command_master = version_master == "9080" ? "src=btn_IGN()" : "scr=btn_ENG()";
    qDebug()<<command_master<<command_slave;
    request_slave.setUrl(QUrl("http://"+address_slave+ ":" + version_slave + "/mandala?"+command_slave));
    manager_slave->get(request_slave);
    request_master.setUrl(QUrl("http://" + address_master+ ":" +version_master+"/mandala?"+command_master));
    manager_master->get(request_master);
}

void MainWindow::takeoff_mode(){
    qDebug()<<"Режим взлета";
    request_slave.setUrl(QUrl("http://"+address_slave+ ":" + version_slave + "/mandala?scr=btn_TAKEOFF()"));
    manager_slave->get(request_slave);
    request_master.setUrl(QUrl("http://" + address_master+ ":" +version_master+"/mandala?scr=btn_TAKEOFF()"));
    manager_master->get(request_master);
}

void MainWindow::thr_cut(){}

void MainWindow::brakes_on_off(){
    static QString breaks_status = "Включены";
    if (breaks_status == "Включены"){ qDebug()<<"Тормоза выключены"; breaks_status = "Выключены";}
    else {qDebug()<<"Тормоза включены"; breaks_status = "Включены";}

    QString command_slave = version_slave == "9080" ? "" : "scr=btn_BRAKE()";
    QString command_master = version_master == "9080" ? "" : "scr=btn_BRAKE()";
    request_slave.setUrl(QUrl("http://"+address_slave+ ":" + version_slave + "/mandala?"+command_slave));
    manager_slave->get(request_slave);
    request_master.setUrl(QUrl("http://" + address_master+ ":" +version_master+"/mandala?"+command_master));
    manager_master->get(request_master);
}

void MainWindow::thr_plus(){
    qDebug()<<"Увеличение оборотов";
    QString command_slave = version_slave == "9080" ? "scr=btn_STAGE()" : "ctr.eng.thr=0.99";
    QString command_master = version_master == "9080" ? "scr=btn_STAGE()" : "ctr.eng.thr=0.99";
    request_master.setUrl(QUrl("http://" + address_master+ ":" +version_master+"/mandala?"+command_master));
    manager_master->get(request_master);
    request_slave.setUrl(QUrl("http://"+address_slave+ ":" + version_slave + "/mandala?"+command_slave));
    manager_slave->get(request_slave);
    request_master.setUrl(QUrl("http://" + address_master+ ":" +version_master+"/mandala?"+command_master));
    manager_master->get(request_master);
}

void MainWindow::on_brakes_btn_clicked()
{
    brakes_on_off();
}


void MainWindow::on_slave_airspeed_valueChanged(int value)
{
    ui->airspeed_slave_lbl->setText(QString::number(value));
}


void MainWindow::on_ign_btn_clicked()
{
    ignition();
}


void MainWindow::on_takeoff_btn_clicked()
{
    takeoff_mode();
}


void MainWindow::on_thr_plus_clicked()
{
    thr_plus();
}


void MainWindow::on_wpt_mode_btn_clicked()
{
    airspeed_ctr_updater->start();
    QString command_master = "cmd.proc.mode=proc_mode_WPT";
    QString command_slave = "cmd.proc.mode=proc_mode_WPT";
    request_slave.setUrl(QUrl("http://"+address_slave+ ":" + version_slave + "/mandala?"+command_slave + "&ctr.eng.thr=0.5"));
    manager_slave->get(request_slave);
    request_master.setUrl(QUrl("http://"+address_master+ ":" + version_master + "/mandala?"+command_master + "&ctr.eng.thr=0.5"));
    manager_master->get(request_master);
}



