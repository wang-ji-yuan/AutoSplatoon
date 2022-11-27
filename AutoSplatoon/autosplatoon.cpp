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

#include <QDebug>
#include <QPixmap>
#include "version.h"


#define VIEW_MAX_WIDTH 320      //视口最大宽度
#define VIEW_MAX_HEIGHT 120     //视口最大高度


AutoSplatoon::AutoSplatoon(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::AutoSplatoon)
{
    ui->setupUi(this);

    ui->statusBar->addWidget(label_state);
    ui->statusBar->setSizeGripEnabled(false);//去掉状态栏右下角的三角
    ui->m_pView->resize(VIEW_MAX_WIDTH,VIEW_MAX_HEIGHT);
    this->m_nViewWidth = ui->m_pView->width();
    this->m_nViewHeight = ui->m_pView->height();
    m_pScene= new QGraphicsScene(this);
    this->m_pScene->setSceneRect(0,0,VIEW_MAX_WIDTH,VIEW_MAX_HEIGHT);


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

    connect(ui->checkBox_ToGray,SIGNAL(clicked()),this,SLOT(checkBox_stateChanged()));
    connect(ui->checkBox_inverse,SIGNAL(clicked()),this,SLOT(checkBox_stateChanged()));
    //    connect(ui->m_pView ,SIGNAL(mouseRelease()),this,SLOT(on_pushButton_Estimate_clicked()));//想做成图像移动(放开鼠标后重新计算时间),没做好

}

AutoSplatoon::~AutoSplatoon()
{
    if(nullptr != m_pScene)
    {
        delete m_pScene;
        m_pScene = nullptr;
    }
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

QImage  AutoSplatoon::ConverToMono( QImage &img,bool Reserved)
{
    QImage imgCopy;

    if (img.format() != QImage::Format_RGB888) {
        imgCopy = QImage(img).convertToFormat(QImage::Format_RGB888);
    } else {
        imgCopy = QImage(img);
    }
    uint8_t *rgb = imgCopy.bits();
    int newGray = 0;
    int gray = 0;
    int size = img.width() * img.height();
    for (int i = 0; i < size ; i++) {
        gray = (rgb[i * 3] + rgb[i * 3 + 1] + rgb[i * 3 + 2]) / 3;
        if (gray > 128)
            newGray = 255;
        else
            newGray = 0;

        newGray=Reserved?255-newGray:newGray;//颜色反转

        rgb[i * 3] = newGray;
        rgb[i * 3 + 1] = newGray;
        rgb[i * 3 + 2] = newGray;
    }
    return imgCopy;
}



QImage  AutoSplatoon::FloydSteinbergDithering(const QImage &image)
{
    int width = image.width();
    int height = image.height();
    QImage ret = image.copy();
    for(int i = 0; i < height - 1; i ++)
    {
        uchar * p = ret.scanLine(i);
        uchar *pNext = ret.scanLine(i + 1);
        for(int j = 0; j < width - 1; j ++)
        {
            int diff;
            if( p[j] > 128 )
            {
                diff = p[j] - 255;
                p[j] = 255;
            }
            else
            {
                diff = p[j];
                p[j] = 0;
            }
            pNext[j] = qBound(0, pNext[j] + diff * 3 / 16, 255);
            p[j + 1] = qBound(0, p[j + 1] + diff * 3 / 16, 255);
            pNext[j + 1] = qBound(0, pNext[j + 1] + diff * 1 / 4, 255);
        }
        p[width - 1] = (p[width - 1] >= 128 ? 255 : 0);
    }
    uchar * p = ret.scanLine(height - 1);
    for(int j = 0; j < width; j ++)
    {
        p[j] = (p[j] >= 128 ? 255 : 0);
    }

    return ret;
}




void AutoSplatoon::executeTask()
{
    int currentDistance,nextDistance,positionX;
    QPen pen;
    pen.setWidth(1);
    pen.setColor(color);
    for(; row < imageDrawing.height(); row++)
    {
        currentDistance=0;
        nextDistance=0;
        ui->rowBox->setValue(row);
        if(row % 2 == 0)
        {
            for (int i=column; i < imageDrawing.width(); i++)
            {

                if(qGray(imageDrawing.pixel(i, row)) < 128)currentDistance=i-column;
                if(row+1<imageDrawing.height()&&qGray(imageDrawing.pixel(i, row+1)) < 128)nextDistance=i-column;
                else nextDistance=0;


            }
            if(currentDistance>0||nextDistance>0)//本行或者下一行有像素点需要画
            {
                positionX=currentDistance>nextDistance?column+currentDistance:column+nextDistance;
                for (; column <= positionX; column++)
                    //            for (; column < imageDrawing.width(); column++)
                {
                    if(haltFlag)
                        return;
                    while(pauseFlag)
                    {
                        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                    }
                    if(qGray(imageDrawing.pixel(column, row)) < 128)
                    {
                        manControl2->sendCommand("A", interval);
                        if(ui->checkBox_showPointer->isChecked()){
                            m_pScene->addEllipse(column,row,1,1,pen);
                        }

                    }
                    if(column<positionX)
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
                if(qGray(imageDrawing.pixel(i, row)) < 128)currentDistance=column-i;
                if(row+1<imageDrawing.height()&&qGray(imageDrawing.pixel(i, row+1)) < 128)nextDistance=column-i;
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
                    if(qGray(imageDrawing.pixel(column, row)) < 128)
                    {
                        manControl2->sendCommand("A", interval);
                        if(ui->checkBox_showPointer->isChecked()){
                            m_pScene->addEllipse(column,row,1,1,pen);
                        }

                    }
                    if(column>positionX)
                    manControl2->sendCommand("Dl", interval);

                    //                    ui->rowBox->setValue(row);//
                    ui->columnBox->setValue(column);

                }
                column += 1;
            }
        }
        manControl2->sendCommand("Dd", interval);


    }
    //    }
    if(ui->checkBox_saveWhenFinish->isChecked())
    {
        manControl2->sendCommand("Sel", interval); //按"-"保存
    }
    on_haltButton_clicked();
}

void AutoSplatoon::on_uploadButton_clicked()
{

    ui->m_pView->SetScaledDefaultValue();//不缩放
    QString fileName = QFileDialog::getOpenFileName(this,tr("打开图片"),QDir::homePath(),tr("支持的所有格式(*.BMP *.JPG *.PNG *.PBM *.TIF);;"
                                                                                        "Bitmap Files(*.BMP *.RLE *.DIB);;"
                                                                                        "JPEG Files(*.JPG *.JPEG *.JPE);;"
                                                                                        "PNG Files(*.PNG) ;;"
                                                                                        "Portable Bit Map Files(*.PBM *.PGM *.PPM *.PNM *.PFM *.PAM);;"

                                                                                        "TIFF Files(*.TIF *.TIFF)"));

    if(NULL == fileName)
    {

        return;
    }
    imageOriginal = QImage(fileName);//原始
    ImageProcess();
    haltFlag = true;
    pauseFlag = false;
    ui->startButton->setEnabled(true);
    ui->label->setEnabled(true);
    ui->label_currentRow->setEnabled(true);
    ui->label_currentColumn->setEnabled(true);
    ui->intervalBox->setEnabled(true);

    ui->rowBox->setEnabled(true);
    ui->columnBox->setEnabled(true);
    ui->checkBox_inverse->setEnabled(true);
    ui->checkBox_ToGray->setEnabled(true);

    on_pushButton_Estimate_clicked();

    ui->pushButton_Estimate->setEnabled(true);
}


void AutoSplatoon::ImageProcess()
{
    imageBeProcessed=imageOriginal;
    m_pScene->clear();
    imageGray=ui->checkBox_ToGray->isChecked()?imageBeProcessed.convertToFormat(QImage::Format_Mono):imageBeProcessed;//灰度处理
    imageMono =ConverToMono(imageGray,ui->checkBox_inverse->isChecked());//黑白反色

    //    imageMono=imageMono.convertToFormat(QImage::Format_Mono);
    QPixmap pixMono = QPixmap::fromImage(imageMono);
    //    QPixmap pixMono = QPixmap::fromImage(imageBeProcessed);
    //获取缩放比例
    this->m_qrDefaultShrinkedRatio=1;
    QGraphicsPixmapItem *pPixItem = new QGraphicsPixmapItem(pixMono);
    pPixItem->setScale(1/this->m_qrDefaultShrinkedRatio);
    pPixItem->setFlag(QGraphicsPixmapItem::ItemIsMovable,true);
    //添加图形项
    m_pScene->addItem(pPixItem);

    //设置场景
    ui->m_pView->setScene(m_pScene);
    ui->m_pView->show();

}
void AutoSplatoon::on_startButton_clicked()
{
    imageDrawing =ui->m_pView->grab(ui->m_pView->sceneRect().toRect()).toImage();
//    imageDrawing=ConverToMono(imageDrawing,false);
    //    imageDrawing.convertTo(QImage::Format_Mono);//截获的图像边上有透明效果,转为黑白后去掉,但是又容易出黑边
//    imageDrawing=imageBeProcessed;
    timer->start();
    m_pScene->clear();//清理之前的内容
    m_pScene= new QGraphicsScene(this);
    m_pScene->addPixmap(QPixmap::fromImage(imageDrawing));
    ui->m_pView->setQMouseEventUsable(false);
    ui->m_pView->setScene(m_pScene);
    ui->m_pView->show();

    timeUsed=0;
    ui->startButton->setEnabled(false);
    ui->pauseButton->setEnabled(true);
    ui->haltButton->setEnabled(true);
    ui->uploadButton->setEnabled(false);

    ui->label->setEnabled(false);
    ui->label_currentRow->setEnabled(false);
    ui->label_currentColumn->setEnabled(false);
    ui->label_currentRow->setText("当前行数");
    ui->label_currentColumn->setText("当前列数");
    ui->intervalBox->setEnabled(false);
    ui->rowBox->setEnabled(false);
    ui->columnBox->setEnabled(false);
    ui->colorSetButton->setEnabled(false);
    ui->checkBox_inverse->setEnabled(false);
    ui->checkBox_ToGray->setEnabled(false);
    ui->pushButton_Estimate->setEnabled(false);
    interval = ui->intervalBox->value();
    row = ui->rowBox->value();
    column = ui->columnBox->value();
    haltFlag = false;

    executeTask();
    timer->stop();
}

void AutoSplatoon::on_pushButton_Estimate_clicked()
{

    QPixmap pix=ui->m_pView->grab(ui->m_pView->sceneRect().toRect());
    imageDrawing = pix.toImage();
    row = ui->rowBox->value();
    column = ui->columnBox->value();
    int time_ms=Estimate()/1000;
    QString timer=QTime(0, 0, 0).addSecs(time_ms).toString(QString::fromLatin1("HH:mm:ss"));
    ui->label_timeTotal->setText(timer);
}

int AutoSplatoon::Estimate()
{
    int currentRrow=row,currentColumn=column;
    int time_ms=0;
    int intervalTime=ui->intervalBox->value();

    int currentDistance,nextDistance,positionX;
    for(; currentRrow < imageDrawing.height(); currentRrow++)
    {
        currentDistance=0;
        nextDistance=0;
        if(currentRrow % 2 == 0)
        {
            for (int i=currentColumn; i < imageDrawing.width(); i++)
            {

                if(qGray(imageDrawing.pixel(i, currentRrow)) < 128)currentDistance=i-currentColumn;
                if(currentRrow+1<imageDrawing.height()&&qGray(imageDrawing.pixel(i, currentRrow+1)) < 128)nextDistance=i-currentColumn;
                else nextDistance=0;

            }
            if(currentDistance>0||nextDistance>0)//本行或者下一行有像素点需要画
            {
                positionX=currentDistance>nextDistance?currentColumn+currentDistance:currentColumn+nextDistance;
                for (; currentColumn <=positionX; currentColumn++)
                {
                    if(qGray(imageDrawing.pixel(currentColumn, currentRrow)) < 128)
                    {
                        time_ms += intervalTime;
                    }
                    if(currentColumn<positionX)
                    time_ms += intervalTime;
                }
                currentColumn -= 1;
            }
        }
        else
        {
            for (int i=currentColumn; i >=0; i--)
            {
                if(qGray(imageDrawing.pixel(i, currentRrow)) < 128)currentDistance=currentColumn-i;
                if(currentRrow+1<imageDrawing.height()&&qGray(imageDrawing.pixel(i, currentRrow+1)) < 128)nextDistance=currentColumn-i;
                else nextDistance=0;
            }
            if(currentDistance>0||nextDistance>0)
            {
                positionX=currentDistance>nextDistance?currentColumn-currentDistance:currentColumn-nextDistance;

                for (; currentColumn >= positionX; currentColumn--)
                {

                    if(qGray(imageDrawing.pixel(currentColumn, currentRrow)) < 128)
                    {
                        time_ms += intervalTime;

                    }
                    if(currentColumn>positionX)
                    time_ms += intervalTime;
                }
                currentColumn += 1;
            }
        }
        time_ms += intervalTime;
    }
    return time_ms*2.0270; // 每次按键后会再发送一个无按键的信号,所以会延时两次（每秒计算并刷新剩余时间的情况下，估计：01：30：34 实际01：30：32）

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
    ui->rowBox->setEnabled(true);
    ui->columnBox->setEnabled(true);
    ui->pauseButton->setText("暂停");
    ui->checkBox_inverse->setEnabled(true);
    ui->checkBox_ToGray->setEnabled(true);
    ui->pushButton_Estimate->setEnabled(false);
    ui->m_pView->setQMouseEventUsable(true);
    ui->label_currentRow->setEnabled(true);
    ui->label_currentColumn->setEnabled(true);
    pauseFlag = false;
    haltFlag = true;
    column = 0; row = 0;
    ui->rowBox->setValue(0);
    ui->columnBox->setValue(0);

    QGraphicsScene *scene = new QGraphicsScene;
    scene = NULL;
    ui->m_pView->setScene(scene);
    ui->m_pView->show();
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


void AutoSplatoon::checkBox_stateChanged()
{
    ImageProcess();
    on_pushButton_Estimate_clicked();
}

void AutoSplatoon::on_helpButton_clicked()
{
    QMessageBox::information(this, "使用说明", "使用步骤：\n1.购买ESP32的开发板\n2.根据开发板串口芯片安装串口驱动\n3.选择串口连接开发板(第一次先烧录)\n4.打开游戏，在广场的邮箱处进入涂鸦界面并按下左摇杆清空所有内容(最好设置比较长的自动休眠时间)\n5.让switch进入“更改握法/排序”界面并等待虚拟的手柄出现\n6.通过软件上“手动控制”界面操作Switch回到游戏涂鸦界面\n7.继续在“手动控制”界面操作,确保游戏界面上光标在左上角,且最小画笔\n8.关闭“手动控制”界面,上传图片\n9.在预览框根据需要移动图片或缩放图片，默认打开灰度效果，关闭黑白反色\n10.灰度效果 黑白反色切换后会自动刷新耗时估算，但移动/缩放后不会，需要手动点击\n11.在预览框看到满意的效果后点击开始就可以了.默认画完后会自动保存并退出涂鸦界面\n 注:\n1.如果看到游戏机上的涂鸦错位了就点击结束,通过“手动控制”界面清空内容，光标回到左上重新开始吧\n2.按键操作间隔数值小速度快但容易导致错位，同时开发板离Switch最好近一点", QMessageBox::Yes, QMessageBox::Yes);
}

void AutoSplatoon::on_savePicButton_clicked()
{
    imageDrawing =ui->m_pView->grab(ui->m_pView->sceneRect().toRect()).toImage();
//    imageDrawing.convertTo(QImage::Format_Mono);//截获的图像边上有透明效果,转为黑白后去掉,但是又容易出黑边
    QString aFileName;
    aFileName=QFileDialog::getSaveFileName(this,tr("保存图像"),QDir::homePath(),"jpg(*.jpg);;png(*.png);;all files(*.*)");
    imageDrawing.save(aFileName);
}
