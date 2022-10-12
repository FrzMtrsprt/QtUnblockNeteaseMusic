#include "mainwindow.h"

#include <QApplication>
#include <QDir>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QSharedMemory>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/res/icon.ico"));
    a.setApplicationName("QtUnblockNeteaseMusic");
    a.setApplicationVersion("1.3.1");
    a.setOrganizationName("FrzMtrsprt");
    a.setOrganizationDomain("https://github.com/FrzMtrsprt/QtUnblockNeteaseMusic");

    QDir::setCurrent(QApplication::applicationDirPath());

    const QLocale locale = QLocale();
    const QString translationsPath =
        QLibraryInfo::path(QLibraryInfo::TranslationsPath);

    // load app translations
    QTranslator appTranslator;
    // look up e.g. :/i18n/QtUnblockNeteaseMusic_en.qm
    if (appTranslator.load(locale, "QtUnblockNeteaseMusic", "_", ":/i18n"))
    {
        a.installTranslator(&appTranslator);
    }

    // load Qt base translations
    QTranslator baseTranslator;
    // look up e.g. {current_path}/translations/qt_en.qm
    if (baseTranslator.load(locale, "qt", "_", translationsPath))
    {
        a.installTranslator(&baseTranslator);
    }

    // reference: https://forum.qt.io/post/504087
    QSharedMemory singular(a.applicationName());

    if (singular.attach(QSharedMemory::ReadOnly))
    {
        // instance already running
        singular.detach();

        const QString title = QObject::tr("Error");
        const QString text =
            a.applicationName() +
            QObject::tr(" is already running.");

        QMessageBox *errorDlg = new QMessageBox();
        errorDlg->setAttribute(Qt::WA_DeleteOnClose);
        errorDlg->setWindowTitle(title);
        errorDlg->setText(text);
        errorDlg->setIcon(QMessageBox::Critical);
        errorDlg->exec();

        return -42;
    }
    else
    {
        // program not yet running
        singular.create(1);
    }

    // don't show window if "-silent" in arguments
    MainWindow w;
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
