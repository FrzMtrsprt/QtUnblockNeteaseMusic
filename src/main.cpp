#include <QApplication>
#include <QDir>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QSharedMemory>
#include <QTranslator>

#include "mainwindow.h"
#include "tray.h"

using namespace Qt::Literals::StringLiterals;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(u"QtUnblockNeteaseMusic"_s);
    a.setApplicationVersion(u"1.3.4"_s);
    a.setOrganizationName(u"FrzMtrsprt"_s);
    a.setOrganizationDomain(u"https://github.com/FrzMtrsprt/QtUnblockNeteaseMusic"_s);

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

    // reference: https://forum.qt.io/post/504087
    QSharedMemory singular(a.applicationName());

    singular.create(1, QSharedMemory::ReadOnly);

    if (singular.error() == QSharedMemory::AlreadyExists)
    {
        // instance already running
        const QString title = QObject::tr("Error");
        const QString text =
            QObject::tr("%1 is already running.")
                .arg(a.applicationName());

        QMessageBox *errorDlg = new QMessageBox();
        errorDlg->setAttribute(Qt::WA_DeleteOnClose);
        errorDlg->setWindowTitle(title);
        errorDlg->setText(text);
        errorDlg->setIcon(QMessageBox::Critical);
        errorDlg->exec();

        return -42;
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
