#include "autosplatoon.h"
#include "ui_autosplatoon.h"
#include <QFile>
#include <QMessageBox>
#include <QRadioButton>
#include <QSerialPortInfo>
#include <QTextStream>
#include <QFileDialog>
#include <QImage>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QtDebug>
#include <QThread>
#include <QProcess>
#include <QElapsedTimer>

AutoSplatoon::AutoSplatoon(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::AutoSplatoon)
{
    ui->setupUi(this);
    fillSerialPorts();

    signalMapper = new QSignalMapper(this);

    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);

    setFixedSize(this->width(),this->height());

    setWindowTitle("AutoSplatoon");

    ui->intervalBox->setValue(70);
    ui->rowBox->setValue(0);
    ui->columnBox->setValue(0);

    manControl2 = new ManualControl();
    manControl2->setAttribute(Qt::WA_DeleteOnClose);
    connect(manControl2, SIGNAL(buttonAction(quint64, bool)), this, SLOT(recieveButtonAction(quint64, bool)));
    connect(manControl2, SIGNAL(manControlDeletedSignal()), this, SLOT(manControlDeletedSignal()));
}

AutoSplatoon::~AutoSplatoon()
{
    delete ui;
}

void AutoSplatoon::fillSerialPorts()
{
    QList<QSerialPortInfo> serialList = QSerialPortInfo::availablePorts();
    ui->serialPortsBox->clear();
    serialPorts.clear();
    for (const QSerialPortInfo& serialPortInfo : serialList) {
        serialPorts.append(serialPortInfo.systemLocation());
    }
    ui->serialPortsBox->addItems(serialPorts);
}

void AutoSplatoon::on_serialRefreshButton_clicked()
{
    fillSerialPorts();
}

void AutoSplatoon::manControlDeletedSignal()
{
    manControlDeleted = true;
}

void AutoSplatoon::on_uploadButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"),QDir::homePath(),tr("(*.bmp)"));
    QImage tmp = QImage(fileName);
    if(tmp.width() == 320 && tmp.height() == 120)
    {
        haltFlag = true;
        //startFlag = false;
        pauseFlag = false;
        image = QImage(fileName);
        QGraphicsScene *scene = new QGraphicsScene;
        scene->addPixmap(QPixmap::fromImage(image));
        ui->graphicsView->setScene(scene);
        ui->graphicsView->show();
        ui->startButton->setEnabled(true);
        ui->label->setEnabled(true);
        ui->label_2->setEnabled(true);
        ui->label_4->setEnabled(true);
        ui->intervalBox->setEnabled(true);
        ui->rowBox->setEnabled(true);
        ui->columnBox->setEnabled(true);
    }
}

void AutoSplatoon::recieveButtonAction(quint64 action, bool temporary)
{
    emit sendButtonAction(action, temporary);
}

void AutoSplatoon::handleSerialStatus(uint8_t status)
{
    if (status == CONNECTING) {
        ui->serialStatusLabel->setText("连接中");
        ui->serialPortsBox->setEnabled(false);
        ui->serialRefreshButton->setEnabled(false);
        ui->uploadButton->setEnabled(false);
        //ui->forceVanillaConnection->setEnabled(false);
        ui->serialConnectButton->setText("断开连接");
    } else if (status == CONNECTED_OK) {
        ui->serialStatusLabel->setText("已连接");
        ui->uploadButton->setEnabled(true);
        ui->flashButton->setEnabled(false);
        //connect(this, SIGNAL(), serialController, SLOT(recieveButtonAction(quint64, bool)));
    } else if (status == CHOCO_SYNCED_JC_L) {
        ui->serialStatusLabel->setText("Connected as Left JoyCon!");
    } else if (status == CHOCO_SYNCED_JC_R) {
        ui->serialStatusLabel->setText("Connected as Right JoyCon!");
    } else if (status == CHOCO_SYNCED_PRO) {
        ui->serialStatusLabel->setText("已连接");
        ui->uploadButton->setEnabled(true);
        ui->flashButton->setEnabled(false);
    } else if (status == CONNECTION_FAILED) {
        ui->serialStatusLabel->setText("连接失败");
    } else {
        ui->serialStatusLabel->setText("断连");
        ui->serialPortsBox->setEnabled(true);
        ui->serialRefreshButton->setEnabled(true);
        ui->uploadButton->setEnabled(false);
        ui->flashButton->setEnabled(true);
        ui->serialConnectButton->setText("连接");
        ui->serialConnectButton->setEnabled(true);
        ui->startButton->setEnabled(false);
        ui->pauseButton->setEnabled(false);
        ui->haltButton->setEnabled(false);
        //ui->forceVanillaConnection->setEnabled(true);
    }
}

void AutoSplatoon::createSerial()
{
    if (ui->serialPortsBox->currentText().isEmpty() == false) {
        if (!ui->serialPortsBox->isEnabled()) {
            ui->serialConnectButton->setEnabled(false);
            //ui->forceVanillaConnection->setEnabled(false);
            serialController->deleteLater();
        } else {
            serialController = new SerialController(this);
            connect(serialController, SIGNAL(reportSerialStatus(uint8_t)), this, SLOT(handleSerialStatus(uint8_t)));
            serialController->openAndSync(ui->serialPortsBox->currentText());
        }
    }
}

void AutoSplatoon::on_serialConnectButton_clicked()
{
    createSerial();
}

QProcess process;
QString output;

void AutoSplatoon::on_readoutput()
{
    output.append(QString(process.readAllStandardOutput().data()));
}

void AutoSplatoon::on_flashButton_clicked()
{
    ui->serialConnectButton->setEnabled(false);
    ui->flashButton->setEnabled(false);
    ui->serialPortsBox->setEnabled(false);
    ui->serialRefreshButton->setEnabled(false);
    ui->manualButton->setEnabled(false);
    ui->serialStatusLabel->setText("烧录中");

    QElapsedTimer timer;
    timer.start();
    QString cmd = QApplication::applicationDirPath();
    cmd += "/esptool.exe";
    qDebug() << cmd;
    QStringList arg;
    arg << "--baud";arg << "230400";arg << "write_flash";arg << "0x0";arg << QApplication::applicationDirPath()+"/PRO-UART0.bin";
//    arg << "--baud";arg << "230400";arg << "write_flash";
//    arg << "0x1000";arg << QApplication::applicationDirPath()+"/bootloader.bin";
//    arg << "0x10000";arg << QApplication::applicationDirPath()+"/firmware.bin";
//    arg << "0x8000";arg << QApplication::applicationDirPath()+"/partition-table.bin";

    process.start(cmd, arg);
    connect(&process , SIGNAL(readyReadStandardOutput()) , this , SLOT(on_readoutput()));
    QEventLoop loop;
    connect(&process,SIGNAL(finished(int,QProcess::ExitStatus)),&loop,SLOT(quit()));
    loop.exec();
    process.kill();
    qDebug() << output;
    int flag = output.indexOf("100 %");
    if(flag == -1)
        ui->serialStatusLabel->setText("烧录失败");
    else
        ui->serialStatusLabel->setText("烧录成功");
    ui->serialConnectButton->setEnabled(true);
    ui->flashButton->setEnabled(true);
    ui->serialPortsBox->setEnabled(true);
    ui->serialRefreshButton->setEnabled(true);
    ui->manualButton->setEnabled(true);

    output = "";
}

void AutoSplatoon::executeTask()
{
    for(; row < image.height(); row++)
    {
        if(row % 2 == 0)
        {
            for (; column < image.width(); column++)
            {
                if(haltFlag)
                    return;
                while(pauseFlag)
                    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                if(qGray(image.pixel(column, row)) < 128)
                    manControl2->sendCommand("A", interval);
                manControl2->sendCommand("Dr", interval);

                ui->rowBox->setValue(row);
                ui->columnBox->setValue(column);
            }
            column -= 1;
        }
        else
        {
            for (; column >= 0; column--)
            {
                if(haltFlag)
                    return;
                while(pauseFlag)
                    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                if(qGray(image.pixel(column, row)) < 128)
                    manControl2->sendCommand("A", interval);
                manControl2->sendCommand("Dl", interval);

                ui->rowBox->setValue(row);
                ui->columnBox->setValue(column);
            }
            column += 1;
        }
        manControl2->sendCommand("Dd", interval);
    }
    //startFlag = false;
    on_haltButton_clicked();
}

void AutoSplatoon::on_startButton_clicked()
{
    ui->startButton->setEnabled(false);
    ui->pauseButton->setEnabled(true);
    ui->haltButton->setEnabled(true);
    ui->uploadButton->setEnabled(false);

    ui->label->setEnabled(false);
//    ui->label_2->setEnabled(false);
//    ui->label_4->setEnabled(false);
    ui->label_2->setText("当前行数");
    ui->label_4->setText("当前列数");
    ui->intervalBox->setEnabled(false);
    ui->rowBox->setEnabled(false);
    ui->columnBox->setEnabled(false);

    interval = ui->intervalBox->value();
    row = ui->rowBox->value();
    column = ui->columnBox->value();

    //column = 0; row = 0;
    //startFlag = true;
    haltFlag = false;

    executeTask();
}

void AutoSplatoon::on_pauseButton_clicked()
{
    if(!pauseFlag)
    {
        ui->pauseButton->setText("继续");
        pauseFlag = true;
    }
    else
    {
        pauseFlag = false;
        ui->pauseButton->setText("暂停");
    }
}

void AutoSplatoon::on_haltButton_clicked()
{
    ui->pauseButton->setEnabled(false);
    ui->haltButton->setEnabled(false);
    ui->uploadButton->setEnabled(true);
    ui->startButton->setEnabled(false);
    ui->label->setEnabled(false);
    ui->label_2->setEnabled(false);
    ui->label_4->setEnabled(false);
    ui->intervalBox->setEnabled(false);
    ui->rowBox->setEnabled(false);
    ui->columnBox->setEnabled(false);
    ui->pauseButton->setText("暂停");
    //startFlag = false;
    pauseFlag = false;
    haltFlag = true;
    column = 0; row = 0;

    QGraphicsScene *scene = new QGraphicsScene;
    scene = NULL;
    ui->graphicsView->setScene(scene);
    ui->graphicsView->show();
}

void AutoSplatoon::on_manualButton_clicked()
{
    if (manControlDeleted)
    {
        manControlDeleted = false;
        manControl1 = new ManualControl();
        manControl1->setAttribute(Qt::WA_DeleteOnClose);
        manControl1->show();
        connect(manControl1, SIGNAL(buttonAction(quint64, bool)), this, SLOT(recieveButtonAction(quint64, bool)));
        connect(manControl1, SIGNAL(manControlDeletedSignal()), this, SLOT(manControlDeletedSignal()));
    }
}
