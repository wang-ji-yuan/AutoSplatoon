#include "autosplatoon.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AutoSplatoon w;
    w.show();
    return a.exec();
}
