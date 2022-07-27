#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("FrzMtrsprt");
    QCoreApplication::setApplicationName("QtUnblockNeteaseMusic");

    //Load app translations
    QTranslator appTranslator;
    if (appTranslator.load(QLocale::system(), "qt", "_", "locale", ".qm"))
    {
        a.installTranslator(&appTranslator);
    }

    //Load Qt base translations
    QTranslator baseTranslator;
    if (baseTranslator.load(QLocale::system(), "qtbase", "_", "locale", ".qm"))
    {
        a.installTranslator(&baseTranslator);
    }
    
    MainWindow w;
    w.show();
    return a.exec();
}
