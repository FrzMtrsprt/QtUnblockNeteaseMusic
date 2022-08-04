#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/res/icon.ico"));
    a.setOrganizationName("FrzMtrsprt");
    a.setApplicationName("QtUnblockNeteaseMusic");
    a.setApplicationVersion("1.0.0");

    // Load app translations
    QTranslator appTranslator;
    // look up e.g. :/QtUnblockNeteaseMusic/qt_en.qm
    if (appTranslator.load(QLocale::system(), "QtUnblockNeteaseMusic", "_", ":/i18n", ".qm"))
    {
        a.installTranslator(&appTranslator);
    }

    // Load Qt base translations
    QTranslator baseTranslator;
    // look up e.g. translations/qt_en.qm
    if (baseTranslator.load(QLocale::system(), "qt", "_", "translations", ".qm"))
    {
        a.installTranslator(&baseTranslator);
    }
    
    MainWindow w;
    w.show();
    return a.exec();
}
