#include "mainwindow.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/res/icon.ico"));
    a.setApplicationName("QtUnblockNeteaseMusic");
    a.setApplicationVersion("1.1.2");
    a.setOrganizationName("FrzMtrsprt");
    a.setOrganizationDomain("https://github.com/FrzMtrsprt/QtUnblockNeteaseMusic");

    // load app translations
    QTranslator appTranslator;
    // look up e.g. :/i18n/QtUnblockNeteaseMusic_en.qm
    if (appTranslator.load(QLocale::system(), "QtUnblockNeteaseMusic", "_", ":/i18n", ".qm"))
    {
        a.installTranslator(&appTranslator);
    }

    // load Qt base translations
    QTranslator baseTranslator;
    // look up e.g. {current_path}/translations/qt_en.qm
    if (baseTranslator.load(QLocale::system(), "qt", "_", a.applicationDirPath() + "/translations", ".qm"))
    {
        a.installTranslator(&baseTranslator);
    }

    // don't show window if "-silent" in arguments
    MainWindow w;
    bool isSilent = false;
    for (int i = 1; i < argc; i++)
    {
        if (!QString::compare(argv[i], "-silent"))
        {
            isSilent = true;
        }
    }
    if (!isSilent)
    {
        w.show();
    }
    return a.exec();
}
