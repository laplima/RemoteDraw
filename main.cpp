#include "mainwindow.h"
#include <QApplication>
#include <QString>
#include <colibry/ORBManager.h>

using namespace colibry;

int main(int argc, char *argv[])
{
    ORBManager orb{argc,argv};
    QApplication a(argc, argv);

    QString name{"rd"};
    if (argc > 1)
        name = argv[1];

    MainWindow w{orb, name};
    w.show();
    return a.exec();
}
