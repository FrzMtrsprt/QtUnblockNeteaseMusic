#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>
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

    // reference: https://forum.qt.io/post/504087
    QSharedMemory singular(a.applicationName());
    if (singular.attach(QSharedMemory::ReadOnly))
    {
        // instance already running
        singular.detach();
        QMessageBox::critical(NULL, QObject::tr("Error"), a.applicationName() + QObject::tr(" is already running."));
        return -42;
    }
    else
    {
        // program not yet running
        singular.create(1);
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
