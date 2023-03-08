#include "mainwindow.h"
#include <QNetworkReply>
#include <QXmlStreamReader>
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for (int i =0; i<8; i++){
        ui->tableWidget->insertColumn(i); // Заголовки
        ui->tableWidget_2->insertColumn(i);
    }

    QStringList headers = {"Характеристика", "Значение", "Характеристика", "Значение", "Характеристика", "Значение", "Характеристика", "Значение"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_2->setHorizontalHeaderLabels(headers);
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);



    // Network Managers
    manager_master = new QNetworkAccessManager();
    manager_slave = new QNetworkAccessManager();
    connect(manager_master, SIGNAL(finished(QNetworkReply*)), this, SLOT(response_master_recieved(QNetworkReply*)));
    connect(manager_slave, SIGNAL(finished(QNetworkReply*)), this, SLOT(response_received(QNetworkReply*)));

    // import settings
    settings = new QSettings("gcu_settings");
    settings->beginGroup("Settings");
    address_master = settings->value("Settings/address_master").toString();
    address_slave = settings->value("Settings/address_slave").toString();
    version_master = settings->value("Settings/version_master").toString();
    version_slave = settings->value("Settings/version_slave").toString();
    freq_int = settings->value("Settings/timer_frequency").toString();

    qDebug()<< settings->value("Settings/address_master").toString()<<
               settings->value("Settings/address_master").toString()<<
               settings->value("Settings/version_master").toString()<<
               settings->value("Settings/version_slave").toString()<<
               freq_int;


    if (address_master.isEmpty() || address_slave.isEmpty()){
        ui->pushButton_6->click();
    }


    ui->label_5->setText("Ведущий GCU: " + address_master);
    ui->label_4->setText("Ведомый GCU: " + address_slave);
    ui->label_3->setText("Частота обновления информации: "+freq_int+"мс");

    qDebug()<<"timer started";
    // Set timer and timer interval
    timer = new QTimer;
    timer ->setInterval(freq_int.toInt());
    timer->start();
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTable()));

}

MainWindow::~MainWindow()
{
    delete ui;
    delete manager_slave;
    delete manager_master;
}


void MainWindow::updateTable(){
    request_slave.setUrl(QUrl("http://"+address_slave+":" + version_slave + "/mandala"));
    manager_slave->get(request_slave);
    request_master.setUrl(QUrl("http://"+address_master+":"+version_master+"/mandala"));
    manager_master->get(request_master);
}

void MainWindow::response_master_recieved(QNetworkReply *reply){
    fill_table(reply, 0);
}

void MainWindow::response_received(QNetworkReply *reply){
    fill_table(reply, 1);
}

void MainWindow::on_pushButton_clicked()
{
    QString command_slave = version_slave == "9080" ? "cmd_altitude=" : "cmd.pos.altitude=";
    QString command_master = version_master == "9080" ? "cmd_altitude=" : "cmd.pos.altitude=";
    request_slave.setUrl(QUrl("http://"+address_slave+ ":" + version_slave + "/mandala?"+command_slave+ui->lineEdit->text()));
    manager_slave->get(request_slave);
    request_master.setUrl(QUrl("http://" + address_master+ ":" +version_master+"/mandala?"+command_master+ui->lineEdit->text()));
    manager_master->get(request_master);
}


void MainWindow::on_pushButton_2_clicked()
{
    QString command_slave = version_slave == "9080" ? "" : "scr=btn_BRAKE()";
    QString command_master = version_master == "9080" ? "" : "scr=btn_BRAKE()";
    request_slave.setUrl(QUrl("http://"+address_slave+ ":" + version_slave + "/mandala?"+command_slave));
    manager_slave->get(request_slave);
    request_master.setUrl(QUrl("http://" + address_master+ ":" +version_master+"/mandala?"+command_master));
    manager_master->get(request_master);
}


void MainWindow::on_pushButton_3_clicked()
{
    request_slave.setUrl(QUrl("http://"+address_slave+ ":" + version_slave + "/mandala?src=btn_TAKEOFF()"));
    manager_slave->get(request_slave);
    request_master.setUrl(QUrl("http://" + address_master+ ":" +version_master+"/mandala?src=btn_TAKEOFF()"));
    manager_master->get(request_master);
}


void MainWindow::on_pushButton_4_clicked()
{
    QString command_slave = version_slave == "9080" ? "src=btn_IGN()" : "src=btn_ENG()";
    QString command_master = version_master == "9080" ? "src=btn_IGN()" : "src=btn_ENG()";
    request_slave.setUrl(QUrl("http://"+address_slave+ ":" + version_slave + "/mandala?"+command_slave));
    manager_slave->get(request_slave);
    request_master.setUrl(QUrl("http://" + address_master+ ":" +version_master+"/mandala?"+command_master));
    manager_master->get(request_master);
}


void MainWindow::on_pushButton_5_clicked()
{
    QString command_slave = version_slave == "9080" ? "src=btn_STAGE()" : "cmd.rc.thr=1";
    QString command_master = version_master == "9080" ? "src=btn_STAGE()" : "cmd.rc.thr=1";
    request_slave.setUrl(QUrl("http://"+address_slave+ ":" + version_slave + "/mandala?"+command_slave));
    manager_slave->get(request_slave);
    request_master.setUrl(QUrl("http://" + address_master+ ":" +version_master+"/mandala?"+command_master));
    manager_master->get(request_master);
}


void MainWindow::on_pushButton_8_clicked()
{
    request_slave.setUrl(QUrl("http://"+address_slave+ ":" + version_slave + "/mandala?src=btn_CUT()"));
    manager_slave->get(request_slave);
    request_master.setUrl(QUrl("http://" + address_master+ ":" +version_master+"/mandala?src=btn_CUT()"));
    manager_master->get(request_master);
}


void MainWindow::on_pushButton_6_clicked()
{
    settings_gcu = new QDialog;
    QGridLayout *layout = new QGridLayout;
    QPushButton *submit = new QPushButton;
    QPushButton *cancel = new QPushButton;

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

    QLabel *info_type = new QLabel;
    info = new QComboBox;
    info_type->setText("Количество отображаемой информации");
    info->addItems(QStringList{"Полная информация", "Cамое важное"} );

    settings_gcu->setModal(true);
    settings_gcu->setWindowTitle("GCU settings");

    connect(submit, SIGNAL(clicked()), this, SLOT(submit_dialog_button_clicked()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(cancel_dialog_button_clicked()));

    settings_gcu->setLayout(layout);

    submit->setText("Сохранить") ;
    cancel->setText("Отмена");

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
    //layout->addWidget(info_type, 6, 1);
    //layout->addWidget(info, 6, 2);
    layout->addWidget(submit, 10, 1);
    settings_gcu->show();
}


void MainWindow::submit_dialog_button_clicked(){
    if (address_master_text->text().isEmpty()){
        address_master_text->setStyleSheet("QLineEdit { background-color: red }");
    }

    timer->stop();
    freq_int = frequency->currentText();
    timer->setInterval(freq_int.toInt());
    timer->start();

    ui->label_3->setText("Частота обновления информации: "+freq_int+"мс");

    version_master = version_master_text->currentText().toInt() == 10? "9080" : "9280";
    version_slave = version_slave_text->currentText().toInt() == 10? "9080": "9280";

    address_master = address_master_text->text();
    address_slave = address_slave_text->text();

    mode_view = info->currentIndex();

    settings->beginGroup("Settings");
    settings->setValue("address_master", address_master);
    settings->setValue("address_slave", address_slave);
    settings->setValue("version_master", version_master);
    settings->setValue("version_slave", version_slave);
    settings->setValue("timer_frequency", frequency->currentText().toInt());
    settings->endGroup();
    qDebug() << settings->fileName();
    qDebug()<<"Saved";
    ui->label_8->setText("Настройки сохранены");
    ui->label_5->setText("Ведущий GCU: " + address_master);
    ui->label_4->setText("Ведомый GCU: " + address_slave);


}

void MainWindow::cancel_dialog_button_clicked(){
    settings_gcu->close();
}


void MainWindow::fill_table(QNetworkReply *reply, int id){
    QTableWidget *table;
    table = id == 0 ? ui->tableWidget_2: ui->tableWidget;

    table->clearContents();
    table->setRowCount(0);
    if (reply->error()) {
        qDebug() << " from GCU "<<id<<reply->errorString();
        return;
    }

    QXmlStreamReader *xmlReader;
    xmlReader = new QXmlStreamReader(reply);
    int row_iter = 0;
    while(!xmlReader->atEnd() && !xmlReader->hasError()) {
        // Read next element
        QXmlStreamReader::TokenType token = xmlReader->readNext();
        if(token == QXmlStreamReader::StartDocument) {
            continue;
        }
        if(token == QXmlStreamReader::StartElement) {
            if (xmlReader->name() == QString("mandala")){
                continue;
            }

            QString command_flyTo_x;
            QString command_flyTo_y;

            QString element_text = xmlReader->readElementText();

            if (xmlReader->name().toString() == "gps_lat" || xmlReader->name().toString() == "est.pos.lat"){
                command_flyTo_x = element_text;
            }
            if (xmlReader->name().toString() == "gps_lon"){
                command_flyTo_y =element_text;
            }


//            if (){
//                    command_flyTo_x = element_text;
//                }
//            if (xmlReader->name().toString() == "est.pos.lon"){
//                    command_flyTo_y = element_text;
//            }



            // EMULATING COMAND TO SLAVE GCU
            if(!(command_flyTo_x.isEmpty() || command_flyTo_y.isEmpty()))
                qDebug()<<"scr=flyTo("+command_flyTo_x+","+command_flyTo_y+")";


            table->insertRow(row_iter);
            table->setItem(row_iter - 100*((int)row_iter/100), row_iter/100*2, new QTableWidgetItem(xmlReader->name().toString()));
            table->setItem(row_iter - 100*((int)row_iter/100), row_iter/100*2 + 1, new QTableWidgetItem(element_text));
            row_iter++;
        }
    }

    delete xmlReader;
}
