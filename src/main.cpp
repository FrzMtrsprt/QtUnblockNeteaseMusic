#include <QCommandLineParser>
#include <QDir>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QThread>
#include <QTranslator>
#include <SingleApplication>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "mainwindow.h"
#include "tray.h"
#include "updatechecker.h"
#include "version.h"

using namespace Qt::StringLiterals;

int main(int argc, char *argv[])
{
    SingleApplication a(argc, argv, true);
    a.setApplicationName(PROJECT_NAME);
    a.setApplicationVersion(PROJECT_VERSION);
    a.setOrganizationName(PROJECT_AUTHOR);
    a.setOrganizationDomain(PROJECT_HOMEPAGE_URL);

    QCommandLineParser parser;
    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);
    const QCommandLineOption silentOption(u"silent"_s);
    parser.addOption(silentOption);
    parser.process(a);

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
    if (baseTranslator.load(locale, u"qt"_s, u"_"_s, translationsPath) ||
        baseTranslator.load(locale, u"qt"_s, u"_"_s, u"translations"_s))
    {
        a.installTranslator(&baseTranslator);
    }

    Config config;

    MainWindow w(&config);

    Tray tray(&w);

    Server server(&config);
    QObject::connect(&server, &Server::out, &w, &MainWindow::on_serverOut);
    QObject::connect(&server, &Server::err, &w, &MainWindow::on_serverErr);
    QObject::connect(&w, &MainWindow::serverClose, &server, &Server::close);
    QObject::connect(&w, &MainWindow::serverRestart, &server, &Server::restart);

    // Start server in another thread
    QThread serverThread;
    server.moveToThread(&serverThread);
    QObject::connect(&serverThread, &QThread::started, &server, &Server::start);
    QObject::connect(&a, &QApplication::aboutToQuit, [&serverThread]
                     { serverThread.quit(); 
                       serverThread.wait(); });
    serverThread.start();

    UpdateChecker updateChecker;
    QObject::connect(&updateChecker, &UpdateChecker::ready, &w, &MainWindow::gotUpdateStatus);

    // Check update in another thread
    QThread updateCheckerThread;
    updateChecker.moveToThread(&updateCheckerThread);
    QObject::connect(&updateCheckerThread, &QThread::started, &updateChecker, &UpdateChecker::checkUpdate);
    QObject::connect(&a, &QApplication::aboutToQuit, [&updateCheckerThread]
                     { updateCheckerThread.quit(); 
                       updateCheckerThread.wait(); });
    updateCheckerThread.start();

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

    // don't show window if "--silent" in arguments
    if (!parser.isSet(silentOption) && !config.startMinimized)
    {
        w.show();
    }

    return a.exec();
}
