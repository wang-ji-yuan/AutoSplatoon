#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mygraphicsview.h"
#include <QMainWindow>
#include <QColorDialog>
#include "commonNames.h"
#include "inputemulator.h"
#include "manualcontrol.h"
#include "serialcontroller.h"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QLabel>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui {
class AutoSplatoon;
}
QT_END_NAMESPACE


//显示模式
enum DisplayModeEnum
{
    TOPLEFT = 0,            //左上角顶点
    XHorizontalMiddle,      //X水平轴居中
    YVerticalMiddle         //Y垂直轴居中
};



class AutoSplatoon : public QMainWindow {
    Q_OBJECT

public:
    AutoSplatoon(QWidget* parent = nullptr);
    ~AutoSplatoon();

private slots:



    void on_serialRefreshButton_clicked();

    void on_uploadButton_clicked();

    void on_serialConnectButton_clicked();

    void handleSerialStatus(uint8_t status);

    void recieveButtonAction(quint64 action, bool temporary);

    void manControlDeletedSignal();

    void on_flashButton_clicked();

    void on_startButton_clicked();

    void on_pauseButton_clicked();

    void on_haltButton_clicked();
    void executeTask();
    void on_manualButton_clicked();
    void on_readoutput();
    void on_colorSetButton_clicked();

    QImage  ConverToMono( QImage &img,bool Reserved);//转为黑白,是否反转
    QImage FloydSteinbergDithering(const QImage &image);


    int Estimate();//用时估计

    void onTimeOut();

    void on_pushButton_Estimate_clicked();
    void ImageProcess();





    void checkBox_stateChanged();



signals:
    void sendButtonAction(quint64 action, bool temporary);
    void sendNewTimerLength(int newLength);
    void sendCurrentTimer(int current, int max, bool notVotes);

private:
    InputEmulator _inpEmu;
    Ui::AutoSplatoon* ui;
    QStringList serialPorts;
    void fillSerialPorts();
    bool test;
    ManualControl* manControl1 = nullptr;
    ManualControl* manControl2 = nullptr;
    bool manControlDeleted = true;
    SerialController* serialController = nullptr;
    void createSerial();
    QSignalMapper* signalMapper;

    QImage imageOriginal,imageBeProcessed,imageGray,imageMono,imageDrawing;//imageGray,imageFloyd,
    //图像处理过程,原始->灰度->抖动->黑白
    QPixmap pixMono;
    QGraphicsScene *m_pScene;
    qreal m_qrDefaultShrinkedRatio; //图形项默认缩放比例
    int m_nViewWidth;
    int m_nViewHeight;


    QColor color=Qt::red;
    QTimer *timer;

    QLabel *label_state=new QLabel();
    int timeUsed=0;
    int interval;
    int column, row;
    bool pauseFlag = false;
    bool haltFlag = true;
};
#endif // MAINWINDOW_H
