#include "mainwindow.h"
#include <QApplication>
#include <colibry/ORBManager.h>

using namespace colibry;

int main(int argc, char *argv[])
{
    ORBManager orb{argc,argv};

    QApplication a(argc, argv);
    MainWindow w{orb};
    w.show();
    return a.exec();
}
