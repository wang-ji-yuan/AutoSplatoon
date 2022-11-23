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
#include <QTime>
#include "version.h"



AutoSplatoon::AutoSplatoon(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::AutoSplatoon)
{
    ui->setupUi(this);

    ui->statusBar->addWidget(label_state);
    ui->statusBar->setSizeGripEnabled(false);//去掉状态栏右下角的三角
    fillSerialPorts();
    signalMapper = new QSignalMapper(this);

    setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint);

    setFixedSize(this->width(),this->height());

    setWindowTitle(SOFTWARE_TITLE);
    timer=new QTimer();
    timer->setInterval(1000);
    connect(timer,SIGNAL(timeout()),this,SLOT(onTimeOut()));
    ui->intervalBox->setValue(70);
    ui->rowBox->setValue(0);
    ui->columnBox->setValue(0);
    ui->colorSetButton->setStyleSheet("background-color: "+ color.name()+ ";");

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
        //        serialPorts.append(serialPortInfo.systemLocation());
        serialPorts.append(serialPortInfo.portName());

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
        //        QGraphicsScene *scene = new QGraphicsScene;
        scene->addPixmap(QPixmap::fromImage(image));
        scene->setSceneRect(0,0,320,120);
        ui->graphicsView->setScene(scene);
        ui->graphicsView->show();
        ui->startButton->setEnabled(true);
        ui->label->setEnabled(true);
        ui->label_currentRow->setEnabled(true);
        ui->label_currentColumn->setEnabled(true);
        ui->intervalBox->setEnabled(true);
        ui->rowBox->setEnabled(true);
        ui->columnBox->setEnabled(true);
        ui->checkBox_inverse->setEnabled(true);
        on_pushButton_Estimate_clicked();
        ui->pushButton_Estimate->setEnabled(true);
    }
    else
    {
        QMessageBox::information(this, "错误", "图像的尺寸不是320x120");
    }

}

void AutoSplatoon::recieveButtonAction(quint64 action, bool temporary)
{
    emit sendButtonAction(action, temporary);
}

void AutoSplatoon::handleSerialStatus(uint8_t status)
{

    switch(status)
    {
    case CONNECTING:
        ui->serialStatusLabel->setText("连接中");
        ui->serialPortsBox->setEnabled(false);
        ui->serialRefreshButton->setEnabled(false);
        ui->uploadButton->setEnabled(false);
        //ui->forceVanillaConnection->setEnabled(false);
        ui->serialConnectButton->setText("断开连接");
        break;
    case CONNECTED_OK:
        ui->serialStatusLabel->setText("已连接");
        ui->uploadButton->setEnabled(true);
        ui->flashButton->setEnabled(false);
        //connect(this, SIGNAL(), serialController, SLOT(recieveButtonAction(quint64, bool)));
        break;
    case CHOCO_SYNCED_JC_L:
        ui->serialStatusLabel->setText("Connected as Left JoyCon!");
        break;
    case CHOCO_SYNCED_JC_R:
        ui->serialStatusLabel->setText("Connected as Right JoyCon!");
        break;
    case CHOCO_SYNCED_PRO:
        ui->serialStatusLabel->setText("已连接");
        ui->uploadButton->setEnabled(true);
        ui->flashButton->setEnabled(false);
        break;
    case CONNECTION_FAILED:
        ui->serialStatusLabel->setText("连接失败");
        break;
    default:
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
        break;
    }
label_state->setText(ui->serialStatusLabel->text());

    //    if (status == CONNECTING)
    //    {
    //        ui->serialStatusLabel->setText("连接中");
    //        ui->serialPortsBox->setEnabled(false);
    //        ui->serialRefreshButton->setEnabled(false);
    //        ui->uploadButton->setEnabled(false);
    //        //ui->forceVanillaConnection->setEnabled(false);
    //        ui->serialConnectButton->setText("断开连接");
    //    }
    //        else if (status == CONNECTED_OK){
    //        ui->serialStatusLabel->setText("已连接");
    //        ui->uploadButton->setEnabled(true);
    //        ui->flashButton->setEnabled(false);
    //        //connect(this, SIGNAL(), serialController, SLOT(recieveButtonAction(quint64, bool)));
    //    }  else if (status == CHOCO_SYNCED_JC_L) {
    //        ui->serialStatusLabel->setText("Connected as Left JoyCon!");
    //    } else if (status == CHOCO_SYNCED_JC_R) {
    //        ui->serialStatusLabel->setText("Connected as Right JoyCon!");
    //    } else if (status == CHOCO_SYNCED_PRO) {
    //        ui->serialStatusLabel->setText("已连接");
    //        ui->uploadButton->setEnabled(true);
    //        ui->flashButton->setEnabled(false);
    //    } else if (status == CONNECTION_FAILED) {
    //        ui->serialStatusLabel->setText("连接失败");
    //    } else {
    //        ui->serialStatusLabel->setText("断连");
    //        ui->serialPortsBox->setEnabled(true);
    //        ui->serialRefreshButton->setEnabled(true);
    //        ui->uploadButton->setEnabled(false);
    //        ui->flashButton->setEnabled(true);
    //        ui->serialConnectButton->setText("连接");
    //        ui->serialConnectButton->setEnabled(true);
    //        ui->startButton->setEnabled(false);
    //        ui->pauseButton->setEnabled(false);
    //        ui->haltButton->setEnabled(false);
    //        //ui->forceVanillaConnection->setEnabled(true);
    //    }
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
    int currentDistance,nextDistance,positionX;
    QPen pen;
    pen.setWidth(1);
    pen.setColor(color);
    if(ui->checkBox_inverse->isChecked())//反色
    {
        for(; row < image.height(); row++)
        {
            currentDistance=0;
            nextDistance=0;
            ui->rowBox->setValue(row);
            if(row % 2 == 0)
            {
                for (int i=column; i < image.width(); i++)
                {
                    if(qGray(image.pixel(i, row)) > 128)currentDistance=i-column;
                    if(row+1<image.height()&&qGray(image.pixel(i, row+1)) > 128)nextDistance=i-column;
                    else nextDistance=0;
                }
                if(currentDistance>0||nextDistance>0)
                {
                    positionX=currentDistance>nextDistance?column+currentDistance:column+nextDistance;
                    for (; column < positionX; column++)
                    {
                        if(haltFlag)
                            return;
                        while(pauseFlag)
                        {
                            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                        }
                        if(qGray(image.pixel(column, row)) > 128)
                        {
                            manControl2->sendCommand("A", interval);
                            if(ui->checkBox_showPointer->isChecked()){
                                scene->addEllipse(column,row,1,1,pen);
                            }

                        }
                        manControl2->sendCommand("Dr", interval);

                        //                    ui->rowBox->setValue(row);行没变
                        ui->columnBox->setValue(column);

                    }
                    column -= 1;
                }
            }
            else
            {
                for (int i=column; i >=0; i--)
                {
                    if(qGray(image.pixel(i, row)) > 128)currentDistance=column-i;
                    if(row+1<image.height()&&qGray(image.pixel(i, row+1)) > 128)nextDistance=column-i;
                    else nextDistance=0;
                }
                if(currentDistance>0||nextDistance>0)//本行或者下一行有像素点需要画
                {
                    positionX=currentDistance>nextDistance?column-currentDistance:column-nextDistance;

                    for (; column >= positionX; column--)
                    {
                        if(haltFlag)
                            return;
                        while(pauseFlag)
                        {
                            //                            QString text=QDateTime::fromMSecsSinceEpoch(QDateTime::currentDateTime().toMSecsSinceEpoch() - startTime.toMSecsSinceEpoch()).toUTC().toString("hh:mm:ss");
                            //                            ui->label_timeElapsed->setText(text);
                            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                        }
                        if(qGray(image.pixel(column, row)) > 128)
                        {
                            manControl2->sendCommand("A", interval);
                            if(ui->checkBox_showPointer->isChecked()){
                                scene->addEllipse(column,row,1,1,pen);
                            }

                        }
                        manControl2->sendCommand("Dl", interval);

                        //                    ui->rowBox->setValue(row);//
                        ui->columnBox->setValue(column);
                    }
                    column += 1;
                }
            }
            manControl2->sendCommand("Dd", interval);
        }
    }
    else
    {
        for(; row < image.height(); row++)
        {
            currentDistance=0;
            nextDistance=0;
            ui->rowBox->setValue(row);
            if(row % 2 == 0)
            {
                for (int i=column; i < image.width(); i++)
                {

                    if(qGray(image.pixel(i, row)) < 128)currentDistance=i-column;
                    if(row+1<image.height()&&qGray(image.pixel(i, row+1)) < 128)nextDistance=i-column;
                    else nextDistance=0;


                }
                if(currentDistance>0||nextDistance>0)//本行或者下一行有像素点需要画
                {
                    positionX=currentDistance>nextDistance?column+currentDistance:column+nextDistance;
                    for (; column < positionX; column++)
                        //            for (; column < image.width(); column++)
                    {
                        if(haltFlag)
                            return;
                        while(pauseFlag)
                        {
                            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                        }
                        if(qGray(image.pixel(column, row)) < 128)
                        {
                            manControl2->sendCommand("A", interval);
                            if(ui->checkBox_showPointer->isChecked()){
                                scene->addEllipse(column,row,1,1,pen);
                            }

                        }
                        manControl2->sendCommand("Dr", interval);

                        //                    ui->rowBox->setValue(row);行没变
                        ui->columnBox->setValue(column);


                    }
                    column -= 1;
                }
            }
            else
            {
                for (int i=column; i >=0; i--)
                {
                    if(qGray(image.pixel(i, row)) < 128)currentDistance=column-i;
                    if(row+1<image.height()&&qGray(image.pixel(i, row+1)) < 128)nextDistance=column-i;
                    else nextDistance=0;
                }
                if(currentDistance>0||nextDistance>0)
                {
                    positionX=currentDistance>nextDistance?column-currentDistance:column-nextDistance;

                    for (; column >= positionX; column--)
                        //                for (; column >= 0; column--)
                    {
                        if(haltFlag)
                            return;
                        while(pauseFlag)
                        {
                            //                            QString text=QDateTime::fromMSecsSinceEpoch(QDateTime::currentDateTime().toMSecsSinceEpoch() - startTime.toMSecsSinceEpoch()).toUTC().toString("hh:mm:ss");
                            //                            ui->label_timeElapsed->setText(text);
                            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                        }
                        if(qGray(image.pixel(column, row)) < 128)
                        {
                            manControl2->sendCommand("A", interval);
                            if(ui->checkBox_showPointer->isChecked()){
                                scene->addEllipse(column,row,1,1,pen);
                            }

                        }
                        manControl2->sendCommand("Dl", interval);

                        //                    ui->rowBox->setValue(row);//
                        ui->columnBox->setValue(column);
                        //                        QString text=QDateTime::fromMSecsSinceEpoch(QDateTime::currentDateTime().toMSecsSinceEpoch() - startTime.toMSecsSinceEpoch()).toUTC().toString("hh:mm:ss");
                        //                        ui->label_timeElapsed->setText(text);
                    }
                    column += 1;
                }
            }
            manControl2->sendCommand("Dd", interval);


        }
    }
    if(ui->checkBox_saveWhenFinish->isChecked())
    {
        manControl2->sendCommand("Sel", interval); //按"-"保存
    }
    on_haltButton_clicked();
}

void AutoSplatoon::on_startButton_clicked()
{
    timer->start();
    timeUsed=0;
    ui->startButton->setEnabled(false);
    ui->pauseButton->setEnabled(true);
    ui->haltButton->setEnabled(true);
    ui->uploadButton->setEnabled(false);

    ui->label->setEnabled(false);
    //    ui->label_currentRow->setEnabled(false);
    //    ui->label_currentColumn->setEnabled(false);
    ui->label_currentRow->setText("当前行数");
    ui->label_currentColumn->setText("当前列数");
    ui->intervalBox->setEnabled(false);
    ui->rowBox->setEnabled(false);
    ui->columnBox->setEnabled(false);
    ui->colorSetButton->setEnabled(false);
    ui->checkBox_inverse->setEnabled(false);
    ui->pushButton_Estimate->setEnabled(false);
    interval = ui->intervalBox->value();
    row = ui->rowBox->value();
    column = ui->columnBox->value();
    haltFlag = false;

    executeTask();
    timer->stop();
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
    ui->label_currentRow->setEnabled(false);
    ui->label_currentColumn->setEnabled(false);
    ui->intervalBox->setEnabled(false);
    ui->rowBox->setEnabled(false);
    ui->columnBox->setEnabled(false);
    ui->pauseButton->setText("暂停");
    ui->checkBox_inverse->setEnabled(true);
    ui->pushButton_Estimate->setEnabled(false);
    ui->checkBox_inverse->setEnabled(false);
    //startFlag = false;
    pauseFlag = false;
    haltFlag = true;
    column = 0; row = 0;
    ui->rowBox->setValue(0);
    ui->columnBox->setValue(0);

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

void AutoSplatoon::on_colorSetButton_clicked()
{
    color = QColorDialog::getColor(Qt::white, this);
    //        qDebug() << "颜色选择" << color << color.name();
    if(color.isValid()){
        ui->colorSetButton->setStyleSheet("background-color: "+ color.name()+ ";");
        //            qDebug() << "m_winColor== " << color.name();
    }


}





int AutoSplatoon::Estimate()
{
    int currentRrow=row,currentColumn=column;
    int time_ms=0;
    int intervalTime=ui->intervalBox->value();

    int currentDistance,nextDistance,positionX;
    if(ui->checkBox_inverse->isChecked())//反色
    {
        for(; currentRrow < image.height(); currentRrow++)
        {
            currentDistance=0;
            nextDistance=0;
            if(currentRrow % 2 == 0)
            {
                for (int i=currentColumn; i < image.width(); i++)
                {
                    if(qGray(image.pixel(i, currentRrow)) > 128)currentDistance=i-currentColumn;
                    if(currentRrow+1<image.height()&&qGray(image.pixel(i, currentRrow+1)) > 128)nextDistance=i-currentColumn;
                    else nextDistance=0;
                }
                if(currentDistance>0||nextDistance>0)
                {
                    positionX=currentDistance>nextDistance?currentColumn+currentDistance:currentColumn+nextDistance;
                    for (; currentColumn < positionX; currentColumn++)
                    {

                        if(qGray(image.pixel(currentColumn, currentRrow)) > 128)
                        {
                            time_ms += intervalTime;

                        }
                        time_ms += intervalTime;
                    }
                    currentColumn -= 1;
                }
            }
            else
            {
                for (int i=currentColumn; i >=0; i--)
                {
                    if(qGray(image.pixel(i, currentRrow)) > 128)currentDistance=currentColumn-i;
                    if(currentRrow+1<image.height()&&qGray(image.pixel(i, currentRrow+1)) > 128)nextDistance=currentColumn-i;
                    else nextDistance=0;
                }
                if(currentDistance>0||nextDistance>0)//本行或者下一行有像素点需要画
                {
                    positionX=currentDistance>nextDistance?currentColumn-currentDistance:currentColumn-nextDistance;
                    for (; currentColumn >= positionX; currentColumn--)
                    {
                        if(qGray(image.pixel(currentColumn, currentRrow)) > 128)
                        {
                            time_ms += intervalTime;
                        }

                        time_ms += intervalTime;
                    }
                    currentColumn += 1;
                }
            }
            time_ms += intervalTime;
        }
    }
    else
    {
        for(; currentRrow < image.height(); currentRrow++)
        {
            currentDistance=0;
            nextDistance=0;
            if(currentRrow % 2 == 0)
            {
                for (int i=currentColumn; i < image.width(); i++)
                {

                    if(qGray(image.pixel(i, currentRrow)) < 128)currentDistance=i-currentColumn;
                    if(currentRrow+1<image.height()&&qGray(image.pixel(i, currentRrow+1)) < 128)nextDistance=i-currentColumn;
                    else nextDistance=0;

                }
                if(currentDistance>0||nextDistance>0)//本行或者下一行有像素点需要画
                {
                    positionX=currentDistance>nextDistance?currentColumn+currentDistance:currentColumn+nextDistance;
                    for (; currentColumn < positionX; currentColumn++)
                    {
                        if(qGray(image.pixel(currentColumn, currentRrow)) < 128)
                        {
                            time_ms += intervalTime;
                        }
                        time_ms += intervalTime;
                    }
                    currentColumn -= 1;
                }
            }
            else
            {
                for (int i=currentColumn; i >=0; i--)
                {
                    if(qGray(image.pixel(i, currentRrow)) < 128)currentDistance=currentColumn-i;
                    if(currentRrow+1<image.height()&&qGray(image.pixel(i, currentRrow+1)) < 128)nextDistance=currentColumn-i;
                    else nextDistance=0;
                }
                if(currentDistance>0||nextDistance>0)
                {
                    positionX=currentDistance>nextDistance?currentColumn-currentDistance:currentColumn-nextDistance;

                    for (; currentColumn >= positionX; currentColumn--)
                    {

                        if(qGray(image.pixel(currentColumn, currentRrow)) < 128)
                        {
                            time_ms += intervalTime;

                        }
                        time_ms += intervalTime;
                    }
                    currentColumn += 1;
                }
            }
            time_ms += intervalTime;
        }
    }
    return time_ms*2.0270; // 每次按键后会再发送一个无按键的信号,所以会延时两次（每秒计算并刷新剩余时间的情况下，估计：01：30：34 实际01：30：32）

}


void AutoSplatoon::on_pushButton_Estimate_clicked()
{
    row = ui->rowBox->value();
    column = ui->columnBox->value();
    int time_ms=Estimate()/1000;
    QString timer=QTime(0, 0, 0).addSecs(time_ms).toString(QString::fromLatin1("HH:mm:ss"));
    ui->label_timeTotal->setText(timer);
}








void AutoSplatoon::onTimeOut()
{
    //刷新的情况下 估计：01：30：34 实际01：30：32 不刷新的情况下实际01：30：30
    //    int time_s=Estimate()/1000;//时间需要X1.014
    //    QString timer=QTime(0, 0, 0).addSecs(time_s).toString(QString::fromLatin1("HH:mm:ss"));
    //    ui->label_timeNeed->setText(timer);
    timeUsed++;
    QString  timer=QTime(0, 0, 0,0).addSecs(timeUsed).toString(QString::fromLatin1("HH:mm:ss"));
    ui->label_timeElapsed->setText(timer);

}


