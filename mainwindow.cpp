#include "mainwindow.h"
#include <QNetworkReply>
#include <QXmlStreamReader>
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->insertColumn(0); // Заголовки
    ui->tableWidget->insertColumn(1); // Значения
    ui->tableWidget->insertColumn(2); // Заголовки
    ui->tableWidget->insertColumn(3); // Значения
    ui->tableWidget->insertColumn(4); // Заголовки
    ui->tableWidget->insertColumn(5); // Значения
    ui->tableWidget->insertColumn(6); // Заголовки
    ui->tableWidget->insertColumn(7); // Значения
    QStringList headers = {"Характеристика", "Значение", "Характеристика", "Значение", "Характеристика", "Значение", "Характеристика", "Значение"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    timer = new QTimer;
    timer ->setInterval(500);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTable()));

    manager = new QNetworkAccessManager();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(response_received(QNetworkReply*)));
    timer->start();



    settings = new QSettings("gcu_settings");
    settings->beginGroup("Connection");

    /// ДОДЕЛАТЬ ЧТЕНИЕ НАСТРОЕК

}

MainWindow::~MainWindow()
{
    delete ui;
    delete manager;
}


void MainWindow::updateTable(){
    request.setUrl(QUrl("http://127.0.0.1:9080/mandala"));
    manager->get(request);
}


void MainWindow::response_received(QNetworkReply *reply){

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    if (reply->error()) {
        qDebug() << reply->errorString();
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
            ui->tableWidget->insertRow(row_iter);

            //qDebug()<<row_iter/100;

            ui->tableWidget->setItem(row_iter - 100*((int)row_iter/100), row_iter/100*2, new QTableWidgetItem(xmlReader->name().toString()));
            ui->tableWidget->setItem(row_iter - 100*((int)row_iter/100), row_iter/100*2 + 1, new QTableWidgetItem(xmlReader->readElementText()));

            row_iter++;
        }
    }

    delete xmlReader;
}

void MainWindow::on_pushButton_clicked()
{
    request.setUrl(QUrl("http://127.0.0.1:9080/mandala?cmd_altitude="+QString(ui->lineEdit->text())));
    manager->get(request) ;
}


void MainWindow::on_pushButton_2_clicked()
{
    updateTable();
}


void MainWindow::on_pushButton_3_clicked()
{
    request.setUrl(QUrl("http://127.0.0.1:9080/mandala?src=btn_TAKEOFF()"));
    manager->get(request);
}


void MainWindow::on_pushButton_4_clicked()
{
    request.setUrl(QUrl("http://127.0.0.1:9080/mandala?src=btn_IGN()"));
    manager->get(request);
}


void MainWindow::on_pushButton_5_clicked()
{
    request.setUrl(QUrl("http://127.0.0.1:9080/mandala?src=btn_STAGE()"));
    manager->get(request);
}


void MainWindow::on_pushButton_8_clicked()
{
    request.setUrl(QUrl("http://127.0.0.1:9080/mandala?src=btn_CUT()"));
    manager->get(request);
}


void MainWindow::on_pushButton_6_clicked()
{
    settings_gcu = new QDialog;
    QGridLayout *layout = new QGridLayout;
    QPushButton *submit = new QPushButton;
    QPushButton *cancel = new QPushButton;

    QLabel *version_slave = new QLabel;
    version_slave_text = new QComboBox;
    version_slave->setText("Версия ведомого GCU");
    version_slave_text->addItems(QStringList{"10", "11"});


    version_master_text = new QComboBox;
    QLabel *version_master = new QLabel;
    version_master->setText("Версия ведущего GCU");
    version_master_text->addItems(QStringList{"10", "11"});


    QLabel *address = new QLabel;
    address_text = new QLineEdit;
    address->setText("Адрес ведущего GCU");
    address_text->setText("192.168.1.168");


    QLabel *timer_freq = new QLabel;
    frequency = new QComboBox;
    timer_freq->setText("Частота обновления информации");
    frequency->addItems(QStringList{"300", "500", "1000", "1500", "2000", "2500"});
    frequency->setCurrentIndex(1);

    QLabel *info_type = new QLabel;
    info = new QComboBox;
    info_type->setText("Количество отображаемой информации");
    info->addItems(QStringList{"Полная информация", "Только самое важное"} );

    settings_gcu->setModal(true);
    settings_gcu->setWindowTitle("GCU settings");

    connect(submit, SIGNAL(clicked()), this, SLOT(submit_dialog_button_clicked()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(cancel_dialog_button_clicked()));

    settings_gcu->setLayout(layout);

    submit->setText("Подтвердить") ;
    cancel->setText("Отмена");

    layout->addWidget(version_slave, 1, 1);
    layout->addWidget(version_slave_text, 1, 2);
    layout->addWidget(version_master, 2, 1);
    layout->addWidget(version_master_text, 2, 2);
    layout->addWidget(address, 3, 1);
    layout->addWidget(address_text, 3, 2);
    layout->addWidget(timer_freq, 4, 1);
    layout->addWidget(frequency, 4, 2);
    layout->addWidget(info_type, 5, 1);
    layout->addWidget(info, 5, 2);
    layout->addWidget(submit, 10, 1);
    layout->addWidget(cancel, 10, 2);
    settings_gcu->show();
}


void MainWindow::submit_dialog_button_clicked(){
    if (address_text->text().isEmpty()){
        address_text->setStyleSheet("QLineEdit { background-color: red }");
    }

    timer->stop();
    timer->setInterval(frequency->currentText().toInt());
    timer->start();


}

void MainWindow::cancel_dialog_button_clicked(){
    settings_gcu->close();
}



