#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("FrzMtrsprt");
    QCoreApplication::setApplicationName("QtUnblockNeteaseMusic");
    MainWindow w;
    w.show();
    return a.exec();
}
