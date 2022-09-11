#include "manualcontrol.h"

#include "ui_manualcontrol.h"
#include <QSignalMapper>
#include <qtimer.h>

void Delay_MSec(unsigned int msec)
{
    QEventLoop loop;
    QTimer::singleShot(msec, &loop, SLOT(quit()));
    loop.exec();
}

ManualControl::ManualControl(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::ManualControl)
{
    ui->setupUi(this);

    QList<QPushButton*> buttons = {
        ui->buttonA,
        ui->buttonB,
        ui->buttonX,
        ui->buttonY,
        ui->buttonUp,
        ui->buttonDown,
        ui->buttonLeft,
        ui->buttonRight,
        ui->buttonCapture,
        ui->buttonHome,
        ui->buttonSelect,
        ui->buttonStart,
        ui->buttonLR,
        ui->buttonCLK,
        ui->buttonL,
        ui->buttonR,
        ui->buttonZL,
        ui->buttonZR,
        ui->buttonZLZR
    };

    QStringList buttonStrings = {
        "A",
        "B",
        "X",
        "Y",
        "Du",
        "Dd",
        "Dl",
        "Dr",
        "Cap",
        "Home",
        "Sel",
        "Str",
        "L+R",
        "CLK",
        "L",
        "R",
        "ZL",
        "ZR",
        "ZL+ZR"
    };

    signalMapper = new QSignalMapper(this);

    for (int i = 0; i < buttons.count(); i++) {
        connect(buttons[i], SIGNAL(released()), this, SLOT(on_buttonReleased()));
        connect(buttons[i], SIGNAL(pressed()), signalMapper, SLOT(map()));
        signalMapper->setMapping(buttons[i], buttonStrings[i]);
    }

    connect(signalMapper, SIGNAL(mapped(const QString&)), this, SLOT(on_buttonPressed(const QString&)));
    //connect(signalMapper, &QSignalMapper::mapped,
    //this, &ManualControl::on_buttonPressed);
}

ManualControl::~ManualControl()
{
    delete ui;
    emit manControlDeletedSignal();
}

void ManualControl::on_buttonReleased()
{
    emit buttonAction(_inpEmu.NO_INPUT, MANUAL);
}

void ManualControl::sendCommand(const QString& text, int interval)
{
    if (text == "A") {
        emit buttonAction(_inpEmu.BTN_A, MANUAL);
    } else if (text == "B") {
        emit buttonAction(_inpEmu.BTN_B, MANUAL);
    } else if (text == "X") {
        emit buttonAction(_inpEmu.BTN_X, MANUAL);
    } else if (text == "Y") {
        emit buttonAction(_inpEmu.BTN_Y, MANUAL);
    } else if (text == "Du") {
        emit buttonAction(_inpEmu.DPAD_U, MANUAL);
    } else if (text == "Dd") {
        emit buttonAction(_inpEmu.DPAD_D, MANUAL);
    } else if (text == "Dl") {
        emit buttonAction(_inpEmu.DPAD_L, MANUAL);
    } else if (text == "Dr") {
        emit buttonAction(_inpEmu.DPAD_R, MANUAL);
    } else if (text == "Home") {
        emit buttonAction(_inpEmu.BTN_HOME, MANUAL);
    } else if (text == "Cap") {
        emit buttonAction(_inpEmu.BTN_CAPTURE, MANUAL);
    } else if (text == "Sel") {
        emit buttonAction(_inpEmu.BTN_MINUS, MANUAL);
    } else if (text == "Str") {
        emit buttonAction(_inpEmu.BTN_PLUS, MANUAL);
    } else if (text == "L+R") {
        emit buttonAction(_inpEmu.BTN_L + _inpEmu.BTN_R, MANUAL);
    } else if (text == "CLK") {
        emit buttonAction(_inpEmu.BTN_LCLICK, MANUAL);
    } else if (text == "L") {
        emit buttonAction(_inpEmu.BTN_L, MANUAL);
    } else if (text == "R") {
        emit buttonAction(_inpEmu.BTN_R, MANUAL);
    } else if (text == "Lu") {
        emit buttonAction(_inpEmu.LSTICK_U, MANUAL);
    } else if (text == "Ld") {
        emit buttonAction(_inpEmu.LSTICK_D, MANUAL);
    } else if (text == "Ll") {
        emit buttonAction(_inpEmu.LSTICK_L, MANUAL);
    } else if (text == "Lr") {
        emit buttonAction(_inpEmu.LSTICK_R, MANUAL);
    } else if (text == "Ru") {
        emit buttonAction(_inpEmu.RSTICK_U, MANUAL);
    } else if (text == "Rd") {
        emit buttonAction(_inpEmu.RSTICK_D, MANUAL);
    } else if (text == "Rl") {
        emit buttonAction(_inpEmu.RSTICK_L, MANUAL);
    } else if (text == "Rr") {
        emit buttonAction(_inpEmu.RSTICK_R, MANUAL);
    } else if (text == "ADu") {
        emit buttonAction(_inpEmu.A_DPAD_U, MANUAL);
    } else if (text == "ADd") {
        emit buttonAction(_inpEmu.A_DPAD_D, MANUAL);
    } else if (text == "ADl") {
        emit buttonAction(_inpEmu.A_DPAD_L, MANUAL);
    } else if (text == "ADr") {
        emit buttonAction(_inpEmu.A_DPAD_R, MANUAL);
    } else {
        emit buttonAction(_inpEmu.NO_INPUT, MANUAL);
    }
    Delay_MSec(interval);
    emit buttonAction(_inpEmu.NO_INPUT, MANUAL);
    Delay_MSec(interval);

    //TODO. buttonAction函数第二个参数怪怪的，改为TEMPORARY似乎无法连续发送数据
    //      暂时先使用手动延时的方法，其实本质是一样的。待修改。
}

void ManualControl::on_buttonPressed(const QString& text)
{
    if (text == "A") {
        emit buttonAction(_inpEmu.BTN_A, MANUAL);
    } else if (text == "B") {
        emit buttonAction(_inpEmu.BTN_B, MANUAL);
    } else if (text == "X") {
        emit buttonAction(_inpEmu.BTN_X, MANUAL);
    } else if (text == "Y") {
        emit buttonAction(_inpEmu.BTN_Y, MANUAL);
    } else if (text == "Du") {
        emit buttonAction(_inpEmu.DPAD_U, MANUAL);
    } else if (text == "Dd") {
        emit buttonAction(_inpEmu.DPAD_D, MANUAL);
    } else if (text == "Dl") {
        emit buttonAction(_inpEmu.DPAD_L, MANUAL);
    } else if (text == "Dr") {
        emit buttonAction(_inpEmu.DPAD_R, MANUAL);
    } else if (text == "Home") {
        emit buttonAction(_inpEmu.BTN_HOME, MANUAL);
    } else if (text == "Cap") {
        emit buttonAction(_inpEmu.BTN_CAPTURE, MANUAL);
    } else if (text == "Sel") {
        emit buttonAction(_inpEmu.BTN_MINUS, MANUAL);
    } else if (text == "Str") {
        emit buttonAction(_inpEmu.BTN_PLUS, MANUAL);
    } else if (text == "L+R") {
        emit buttonAction(_inpEmu.BTN_L + _inpEmu.BTN_R, MANUAL);
    } else if (text == "CLK") {
        emit buttonAction(_inpEmu.BTN_LCLICK, MANUAL);
    } else if (text == "L") {
        emit buttonAction(_inpEmu.BTN_L, MANUAL);
    } else if (text == "R") {
        emit buttonAction(_inpEmu.BTN_R, MANUAL);
    } else if (text == "ZL") {
        emit buttonAction(_inpEmu.BTN_ZL, MANUAL);
    } else if (text == "ZR") {
        emit buttonAction(_inpEmu.BTN_ZR, MANUAL);
    } else if (text == "ZL+ZR") {
        emit buttonAction(_inpEmu.BTN_ZL + _inpEmu.BTN_ZR, MANUAL);
    } else {
        emit buttonAction(_inpEmu.NO_INPUT, MANUAL);
    }
}
