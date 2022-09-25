#include "config.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QCloseEvent>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <QRegularExpression>
#include <QStyle>
#include <QStyleFactory>
#include <QSystemTrayIcon>

#ifdef Q_OS_WIN32
#include "../utils/winutils.h"
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // setup system tray
    QSystemTrayIcon *tray = new QSystemTrayIcon(this);
    tray->setIcon(QIcon(":/res/icon.ico"));
    tray->setToolTip("QtUnblockNeteaseMusic");
    QMenu *trayMenu = new QMenu(this);
    QAction *trayExit = new QAction(this);
    QAction *trayShow = new QAction(this);
    trayShow->setText(tr("Show"));
    trayExit->setText(tr("Exit"));
    trayMenu->addAction(trayShow);
    trayMenu->addAction(trayExit);
    tray->setContextMenu(trayMenu);
    tray->show();

    // connect tray signals
    connect(trayShow, &QAction::triggered,
            this, &MainWindow::on_show);
    connect(trayExit, &QAction::triggered,
            this, &MainWindow::on_exit);
    // show MainWindow only when tray icon is left clicked
    connect(tray, &QSystemTrayIcon::activated, this,
            [this](QSystemTrayIcon::ActivationReason reason)
            {
                if (reason == QSystemTrayIcon::Trigger)
                {
                    on_show();
                }
            });

    // connect MainWindow signals
    connect(ui->actionExit, &QAction::triggered,
            this, &MainWindow::on_exit);
    connect(ui->actionAbout, &QAction::triggered,
            this, &MainWindow::on_about);
    connect(ui->actionAboutQt, &QAction::triggered,
            this, &MainWindow::on_aboutQt);
    connect(ui->startupCheckBox, &QCheckBox::stateChanged,
            this, &MainWindow::on_startup);
    connect(ui->applyBtn, &QPushButton::clicked,
            this, &MainWindow::on_apply);
    connect(ui->exitBtn, &QPushButton::clicked,
            this, &MainWindow::on_exit);

    // setup theme menu
    for (QString &style : QStyleFactory::keys())
    {
        // reference: https://stackoverflow.com/a/45265455
        QAction *action = ui->menuTheme->addAction(style);
        connect(action, &QAction::triggered,
                this, [this, style]
                { setTheme(style); });
    }

    // setup & load config
    config = new Config();
    loadSettings();

    // setup & start server
    qDebug() << "---Starting server---";
    server = new QProcess();
    connect(server, &QProcess::readyReadStandardOutput,
            this, &MainWindow::on_readoutput);
    connect(server, &QProcess::readyReadStandardError,
            this, &MainWindow::on_readerror);
    startServer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setTheme(const QString &theme)
{
    qDebug() << "Setting theme" << theme;
    QStyle *style = QStyleFactory::create(theme);

#ifdef Q_OS_WIN32
    WinUtils::setWindowFrame(winId(), theme);
#endif
    QApplication::setStyle(style);
    QApplication::setPalette(style->standardPalette());
}

void MainWindow::on_show()
{
#ifdef Q_OS_WIN32
    // Disable power throttling
    WinUtils::setThrottle(false);
#endif

    this->show();
    this->activateWindow();
}

void MainWindow::on_exit()
{
    qDebug() << "---Shutting down---";
    server->close();
    updateSettings();
    QApplication::exit();
}

void MainWindow::on_about()
{
    const QPixmap logo =
        QPixmap(":/res/icon.png")
            .scaledToHeight(100, Qt::SmoothTransformation);

    const QString text =
        tr("<h3>About %1</h3>"
           "<p>Version %2</p>")
            .arg(QApplication::applicationName(),
                 QApplication::applicationVersion());

    const QString info =
        tr("<p>A desktop client for UnblockNeteaseMusic, "
           "written in Qt.</p>"
           "<p>Copyright 2022 %1</p>")
            .arg(QApplication::organizationName());

    QMessageBox *aboutDlg = new QMessageBox(this);
    aboutDlg->setWindowTitle("About");
    aboutDlg->setIconPixmap(logo);
    aboutDlg->setText(text);
    aboutDlg->setInformativeText(info);
    aboutDlg->setStandardButtons(QMessageBox::Ok |
                                 QMessageBox::Help);
    aboutDlg->setEscapeButton(QMessageBox::Ok);
    aboutDlg->button(QMessageBox::Help)->setText("GitHub");
#ifdef Q_OS_WIN32
    const QString theme = QApplication::style()->name();
    WinUtils::setWindowFrame(aboutDlg->winId(), theme);
#endif

    if (aboutDlg->exec() == QMessageBox::Help)
    {
        const QUrl url(QApplication::organizationDomain());
        QDesktopServices::openUrl(url);
    }
}

void MainWindow::on_aboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_startup()
{
    const bool state = ui->startupCheckBox->isChecked();
#ifdef Q_OS_WIN32
    WinUtils::setStartup(state);
#endif
}

void MainWindow::on_readoutput()
{
    const QByteArray log = server->readAllStandardOutput();
    ui->outText->append(log);
}

void MainWindow::on_readerror()
{
    const QByteArray log = server->readAllStandardError();

    QMessageBox *errorDlg = new QMessageBox(this);
    errorDlg->setWindowTitle(tr("Server error"));
    errorDlg->setText(
        tr("The UnblockNeteaseMusic server ran into an error.\n"
           "Please change the arguments or "
           "check port usage and try again."));
    errorDlg->setDetailedText(log);
    errorDlg->setIcon(QMessageBox::Warning);
#ifdef Q_OS_WIN32
    const QString theme = QApplication::style()->name();
    WinUtils::setWindowFrame(errorDlg->winId(), theme);
#endif
    errorDlg->exec();
}

void MainWindow::on_apply()
{
    qDebug() << "---Restarting server---";
    server->close();
    ui->outText->clear();
    startServer();
}

void MainWindow::loadSettings()
{
    qDebug() << "Loading settings";

    // load settings from file into variables
    config->readSettings();

    // load settings from variables into ui
    ui->portEdit->setText(config->port);
    ui->addressEdit->setText(config->address);
    ui->urlEdit->setText(config->url);
    ui->hostEdit->setText(config->host);
    ui->sourceEdit->append(config->sources.join(", "));
    ui->strictCheckBox->setChecked(config->strict);
    ui->startupCheckBox->setChecked(config->startup);
    setTheme(config->theme);

    qDebug() << "Load settings done";
}

void MainWindow::updateSettings()
{
    qDebug() << "Updating settings";

    static const QRegularExpression sep("\\W+");

    // update settings from ui into variables
    config->port = ui->portEdit->text();
    config->address = ui->addressEdit->text();
    config->url = ui->urlEdit->text();
    config->host = ui->hostEdit->text();
    config->sources = ui->sourceEdit->toPlainText()
                          .split(sep, Qt::SkipEmptyParts);
    config->strict = ui->strictCheckBox->isChecked();
    config->startup = ui->startupCheckBox->isChecked();
    config->theme = QApplication::style()->name();

    // write settings from variables into file
    config->writeSettings();

    qDebug() << "Update settings done";
}

bool MainWindow::getServer(QString &program, QStringList &arguments)
{
    const QDir appDir(QApplication::applicationDirPath());
    const QFileInfoList results = appDir.entryInfoList(
        {"unblock*", "server*"},
        QDir::Dirs | QDir::Files | QDir::NoSymLinks);

    for (const QFileInfo &result : results)
    {
        if (result.isFile())
        {
            program = result.absoluteFilePath();
            arguments = {};
            return true;
        }
        if (result.isDir())
        {
            const QString serverDir =
                result.absoluteFilePath();

            if (QDir(serverDir).exists("app.js"))
            {
                program = "node";
                arguments = {serverDir + "/app.js"};
                return true;
            }
        }
    }
    return false;
}

void MainWindow::getArgs(QStringList &arguments)
{
    updateSettings();

    if (!config->port.isEmpty())
    {
        arguments << "-p" << config->port;
    }
    if (!config->address.isEmpty())
    {
        arguments << "-a" << config->address;
    }
    if (!config->url.isEmpty())
    {
        arguments << "-u" << config->url;
    }
    if (!config->host.isEmpty())
    {
        arguments << "-f" << config->host;
    }
    if (!config->sources.isEmpty())
    {
        arguments << "-o" << config->sources;
    }
    if (config->strict)
    {
        arguments << "-s";
    }
}

void MainWindow::startServer()
{
    QString program = "";
    QStringList arguments = {};

    if (getServer(program, arguments))
    {
        getArgs(arguments);
        qDebug() << "Server program:"
                 << program;
        qDebug() << "Server arguments:"
                 << arguments.join(" ");
        server->start(program, arguments);
        if (!server->waitForStarted())
        {
            ui->outText->append(server->errorString());
        }
    }
    else
    {
        ui->outText->append(tr("Server not found."));
    }
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    this->hide();
    e->ignore();

#ifdef Q_OS_WIN32
    // Enable power throttling
    WinUtils::setThrottle(true);
#endif
}
