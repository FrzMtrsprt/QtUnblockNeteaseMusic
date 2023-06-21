#include <QApplication>
#include <QDir>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QSharedMemory>
#include <QTranslator>
#include <SingleApplication>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "mainwindow.h"
#include "tray.h"

using namespace Qt::Literals::StringLiterals;

int main(int argc, char *argv[])
{
    SingleApplication a(argc, argv, true);
    a.setApplicationName(u"QtUnblockNeteaseMusic"_s);
    a.setApplicationVersion(u"1.5.0"_s);
    a.setOrganizationName(u"FrzMtrsprt"_s);
    a.setOrganizationDomain(u"https://github.com/FrzMtrsprt/QtUnblockNeteaseMusic"_s);

    if (a.isSecondary())
    {
#ifdef Q_OS_WIN
        AllowSetForegroundWindow((DWORD)a.primaryPid());
#endif
        if (a.sendMessage(" "_qba))
        {
            return -1;
        }
    }

    QDir::setCurrent(QApplication::applicationDirPath());

    const QLocale locale = QLocale();
    const QString translationsPath =
        QLibraryInfo::path(QLibraryInfo::TranslationsPath);

    // load app translations
    QTranslator appTranslator;
    // look up e.g. :/i18n/QtUnblockNeteaseMusic_en.qm
    if (appTranslator.load(locale, u"QtUnblockNeteaseMusic"_s, u"_"_s, u":/i18n"_s))
    {
        a.installTranslator(&appTranslator);
    }

    // load Qt base translations
    QTranslator baseTranslator;
    // look up e.g. {current_path}/translations/qt_en.qm
    if (baseTranslator.load(locale, u"qt"_s, u"_"_s, translationsPath))
    {
        a.installTranslator(&baseTranslator);
    }

    MainWindow w;

    Tray tray(&w);

    // Open when second instance started
    QObject::connect(&a, &SingleApplication::receivedMessage, &w, [&w]
                     {
#ifdef Q_OS_WIN
                         ShowWindow((HWND)w.winId(), SW_RESTORE);
#endif
                         w.show();
                         w.activateWindow(); });

    // Disable proxy before quit or shutdown
    QObject::connect(&a, &QApplication::aboutToQuit, &w, [&w]
                     { if (w.isProxy()) w.setProxy(false); });

    // Don't quit when closing dialog
    a.setQuitOnLastWindowClosed(false);

    tray.setVisible(true);

    // don't show window if "-silent" in arguments
    bool silent = false;
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-silent") == 0)
        {
            silent = true;
        }
    }
    if (!silent)
    {
        w.show();
    }

    return a.exec();
}
