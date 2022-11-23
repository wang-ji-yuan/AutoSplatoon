#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColorDialog>
#include "commonNames.h"
#include "inputemulator.h"
#include "manualcontrol.h"
#include "serialcontroller.h"
#include <QGraphicsScene>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class AutoSplatoon;
}
QT_END_NAMESPACE

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



    int Estimate();//用时估计

    void onTimeOut();

    void on_pushButton_Estimate_clicked();



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

    QImage image;
     QGraphicsScene *scene = new QGraphicsScene;
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
