#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("FrzMtrsprt");
    QCoreApplication::setApplicationName("QtUnblockNeteaseMusic");

    // Load app translations
    QTranslator appTranslator;
    // look up e.g. :/i18n/qt_en.qm
    if (appTranslator.load(QLocale::system(), "qt", "_", ":/i18n", ".qm"))
    {
        a.installTranslator(&appTranslator);
    }

    // Load Qt base translations
    QTranslator baseTranslator;
    // look up e.g. :/i18n/qtbase_en.qm
    if (baseTranslator.load(QLocale::system(), "qtbase", "_", ":/i18n", ".qm"))
    {
        a.installTranslator(&baseTranslator);
    }
    
    MainWindow w;
    w.show();
    return a.exec();
}
