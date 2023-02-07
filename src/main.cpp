#include <QApplication>
#include <QDir>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QSharedMemory>
#include <QTranslator>
#include <QtSingleApplication>

#include "mainwindow.h"
#include "tray.h"

using namespace Qt::Literals::StringLiterals;

int main(int argc, char *argv[])
{
    QtSingleApplication a(u"QtUnblockNeteaseMusic"_s, argc, argv);
    a.setApplicationName(u"QtUnblockNeteaseMusic"_s);
    a.setApplicationVersion(u"1.3.4"_s);
    a.setOrganizationName(u"FrzMtrsprt"_s);
    a.setOrganizationDomain(u"https://github.com/FrzMtrsprt/QtUnblockNeteaseMusic"_s);

    if (a.isRunning() && a.sendMessage(u""_s))
    {
        return -1;
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

    Tray tray;

    // connect tray signals
    QObject::connect(tray.show, &QAction::triggered, &w, [&w]
                     { w.show(true); });
    QObject::connect(tray.proxy, &QAction::triggered, &w, [&w](const bool &checked)
                     { w.setProxy(checked); });
    QObject::connect(tray.exit, &QAction::triggered, &w, [&w]
                     { w.exit(); });
    QObject::connect(&tray, &Tray::activated, &w, [&w, &tray]
                     { tray.proxy->setChecked(w.isProxy()); });
    QObject::connect(&tray, &Tray::clicked, &w, [&w]
                     { w.show(w.isHidden()); });

    // Open existing instance
    QObject::connect(&a, &QtSingleApplication::messageReceived, &w, [&w]
                     { w.show(true); });

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
    w.show(!silent);

    return a.exec();
}
