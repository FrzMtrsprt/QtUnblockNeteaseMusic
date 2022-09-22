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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
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
    connect(trayShow, &QAction::triggered, this, &MainWindow::on_show);
    connect(trayExit, &QAction::triggered, this, &MainWindow::on_exit);
    // show MainWindow only when tray icon is left clicked
    connect(tray, &QSystemTrayIcon::activated, this,
            [this](QSystemTrayIcon::ActivationReason reason)
            { if (reason == QSystemTrayIcon::Trigger) on_show(); });

    // connect MainWindow signals
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::on_exit);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::on_about);
    connect(ui->actionAboutQt, &QAction::triggered, this, &MainWindow::on_aboutQt);
    connect(ui->startupCheckBox, &QCheckBox::stateChanged, this, &MainWindow::on_startupChanged);
    connect(ui->applyBtn, &QPushButton::clicked, this, &MainWindow::on_apply);
    connect(ui->exitBtn, &QPushButton::clicked, this, &MainWindow::on_exit);

    // setup theme menu
    for (QString &style : QStyleFactory::keys())
    {
        qDebug() << "Loading theme" << style;
        // reference: https://stackoverflow.com/a/45265455
        connect(ui->menuTheme->addAction(style), &QAction::triggered,
                this, [this, style]
                { setTheme(style); });
    }

    // setup & load config
    config = new Config();
    loadSettings();

    // setup & start server
    server = new QProcess();
    connect(server, &QProcess::readyReadStandardOutput, this, &MainWindow::on_readoutput);
    connect(server, &QProcess::readyReadStandardError, this, &MainWindow::on_readerror);
    startServer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setTheme(const QString &theme)
{
    qDebug() << "Setting theme" << theme;
#ifdef Q_OS_WIN32
    WinUtils::setWindowFrame((HWND)winId(), theme);
#endif
    QApplication::setStyle(QStyleFactory::create(theme));
    QApplication::setPalette(QApplication::style()->standardPalette());
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
    server->close();
    updateSettings();
    QApplication::exit();
}

void MainWindow::on_about()
{
    QMessageBox *aboutDlg = new QMessageBox(this);
    const QString text = tr("<h3>About QtUnblockNeteaseMusic</h3>"
                            "<p>Version %1</p>")
                             .arg(QApplication::applicationVersion());
    const QString informativeText = tr("<p>A desktop client for UnblockNeteaseMusic, "
                                       "written in Qt.</p>"
                                       "<p>Copyright 2022 FrzMtrsprt</p>");
    aboutDlg->setWindowTitle("About");
    aboutDlg->setIconPixmap(QPixmap(":/res/icon.png").scaledToHeight(100, Qt::SmoothTransformation));
    aboutDlg->setText(text);
    aboutDlg->setInformativeText(informativeText);
    aboutDlg->setStandardButtons(QMessageBox::Ok | QMessageBox::Help);
    aboutDlg->setEscapeButton(QMessageBox::Ok);
    aboutDlg->button(QMessageBox::Help)->setText("GitHub");
    if (aboutDlg->exec() == QMessageBox::Help)
    {
        QDesktopServices::openUrl(QUrl(QApplication::organizationDomain()));
    }
}

void MainWindow::on_aboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_startupChanged(const int &state)
{
#ifdef Q_OS_WIN32
    WinUtils::setStartup(state);
#endif
}

void MainWindow::on_readoutput()
{
    const QByteArray &logArray = server->readAllStandardOutput();
    ui->outText->append(logArray.data());
}

void MainWindow::on_readerror()
{
    QMessageBox *errorDlg = new QMessageBox(this);
    const QByteArray &logArray = server->readAllStandardError();
    errorDlg->setWindowTitle(tr("Server error"));
    errorDlg->setText(tr("The UnblockNeteaseMusic server ran into an error.\n"
                         "Please change the arguments or check port usage and try again."));
    errorDlg->setDetailedText(logArray.data());
    errorDlg->setIcon(QMessageBox::Warning);
    errorDlg->exec();
}

void MainWindow::on_apply()
{
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
    config->sources = ui->sourceEdit->toPlainText().split(sep, Qt::SkipEmptyParts);
    config->strict = ui->strictCheckBox->isChecked();
    config->startup = ui->startupCheckBox->isChecked();
    config->theme = QApplication::style()->name();

    // write settings from variables into file
    config->writeSettings();

    qDebug() << "Update settings done";
}

bool MainWindow::getServer(QString &serverFile, QStringList &serverArgs)
{
    const QDir appDir(QApplication::applicationDirPath());
    const QFileInfoList results = appDir.entryInfoList(
        {"unblock*", "server*"},
        QDir::Dirs | QDir::Files | QDir::NoSymLinks);
    for (const QFileInfo &result : results)
    {
        if (result.isFile())
        {
            serverFile = result.absoluteFilePath();
            serverArgs = {};
            qDebug() << "Server File:"
                     << serverFile.toUtf8().data();
            return true;
        }
        if (result.isDir())
        {
            const QDir serverDir(result.absoluteFilePath());
            if (serverDir.exists("app.js"))
            {
                serverFile = "node";
                serverArgs = {serverDir.absolutePath() + "/app.js"};
                qDebug() << "Server File:"
                         << serverFile.toUtf8().data();
                return true;
            }
        }
    }
    return false;
}

void MainWindow::getArgs(QStringList &serverArgs)
{
    updateSettings();
    if (!config->port.isEmpty())
    {
        serverArgs << "-p" << config->port;
    }
    if (!config->address.isEmpty())
    {
        serverArgs << "-a" << config->address;
    }
    if (!config->url.isEmpty())
    {
        serverArgs << "-u" << config->url;
    }
    if (!config->host.isEmpty())
    {
        serverArgs << "-f" << config->host;
    }
    if (!config->sources.isEmpty())
    {
        serverArgs << "-o" << config->sources;
    }
    if (config->strict)
    {
        serverArgs << "-s";
    }
    qDebug() << "Server Arguments:"
             << serverArgs.join(" ").toUtf8().data();
}

void MainWindow::startServer()
{
    QString serverFile = "";
    QStringList serverArgs = {};
    if (getServer(serverFile, serverArgs))
    {
        getArgs(serverArgs);
        server->start(serverFile, serverArgs);
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
