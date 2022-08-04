#include "config.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <QRegularExpression>
#include <QStyleFactory>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(server, SIGNAL(readyReadStandardOutput()), this, SLOT(on_readoutput()));
    connect(server, SIGNAL(readyReadStandardError()), this, SLOT(on_readerror()));
    config->readSettings();
    loadSettings();
    startServer();

    // setup system tray
    tray = new QSystemTrayIcon(this);
    tray->setIcon(QIcon(":/res/icon.ico"));
    tray->setToolTip("QtUnblockNeteaseMusic");
    trayMenu = new QMenu(this);
    trayExit = new QAction(this);
    trayShow = new QAction(this);
    trayShow->setText(tr("Show"));
    trayExit->setText(tr("Exit"));
    trayMenu->addAction(trayShow);
    trayMenu->addAction(trayExit);
    tray->setContextMenu(trayMenu);
    tray->show();
    connect(trayShow, &QAction::triggered, this, &MainWindow::show);
    connect(trayExit, &QAction::triggered, this, [this]()
            {
        tray->hide();
        stopServer();
        qApp->quit(); });
    connect(tray, &QSystemTrayIcon::activated, this, &MainWindow::on_tray_activated);

    // setup theme combo box
    ui->themeComboBox->addItems(QStyleFactory::keys());
    connect(ui->themeComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(on_theme_changed(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    stopServer();
    qApp->exit();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox aboutDlg(this);
    QString text = QString(tr("QtUnblockNeteaseMusic\n"
                              "Version %1\n\n"
                              "A desktop client for UnblockNeteaseMusic,\n"
                              "written in Qt.\n\n"
                              "Copyright 2022 FrzMtrsprt"))
                       .arg(versionStr);
    aboutDlg.setWindowTitle("About");
    aboutDlg.setIconPixmap(QPixmap(":/res/icon.png").scaledToHeight(100, Qt::SmoothTransformation));
    aboutDlg.setText(text);
    QPushButton *webBtn = aboutDlg.addButton("GitHub", QMessageBox::HelpRole);
    aboutDlg.setStandardButtons(QMessageBox::Ok);
    aboutDlg.setModal(true);
    aboutDlg.exec();
    if (aboutDlg.clickedButton() == (QAbstractButton *)webBtn)
    {
        QDesktopServices::openUrl(repoUrl);
    }
}

void MainWindow::on_theme_changed(QString theme)
{
    QApplication::setStyle(QStyleFactory::create(theme));
    QApplication::setPalette(QApplication::style()->standardPalette());
}

void MainWindow::on_readoutput()
{
    ui->outText->append(server->readAllStandardOutput().data());
}

void MainWindow::on_readerror()
{
    QMessageBox errorDlg;
    errorDlg.setWindowTitle(tr("Server error"));
    errorDlg.setText(tr("The UnblockNeteaseMusic server ran into an error.\n"
                        "Please change the arguments or check port usage and try again."));
    errorDlg.setDetailedText(server->readAllStandardError().data());
    errorDlg.setIcon(QMessageBox::Warning);
    errorDlg.setStandardButtons(QMessageBox::Ok);
    errorDlg.exec();
}

void MainWindow::on_restartBtn_clicked()
{
    stopServer();
    ui->outText->clear();
    startServer();
}

void MainWindow::on_exitBtn_clicked()
{
    stopServer();
    qApp->exit();
}

void MainWindow::loadSettings()
{
    // load settings from variables into ui
    ui->portEdit->setText(config->port);
    ui->addressEdit->setText(config->address);
    ui->urlEdit->setText(config->url);
    ui->hostEdit->setText(config->host);
    ui->sourceEdit->append(config->source);
    ui->strictCheck->setChecked(config->strict);
}

void MainWindow::updateSettings()
{
    // update settings from ui into variables
    config->port = ui->portEdit->text();
    config->address = ui->addressEdit->text();
    config->url = ui->urlEdit->text();
    config->host = ui->hostEdit->text();
    config->source = ui->sourceEdit->toPlainText();
    config->strict = ui->strictCheck->isChecked();
}

void MainWindow::getPath()
{
    QDir serverDir(QCoreApplication::applicationDirPath());
    QStringList filters;
    filters << "unblock*"
            << "server*";
    serverDir.setNameFilters(filters);
    if (serverDir.count())
    {
        QFileInfo result(serverDir[0]);
        if (result.isFile())
        {
            serverFile = "./" + serverDir[0];
            serverArgs = {};
        }
        if (result.isDir())
        {
            serverFile = "node";
            serverArgs = {"./" + serverDir[0] + "/app.js"};
        }
        qDebug() << "Server File:" << serverFile.toUtf8().data();
    }
    else
    {
        serverFile = "";
        serverArgs = {};
    }
}

void MainWindow::getArgs()
{
    if (serverFile == "")
    {
        return;
    }
    if (ui->portEdit->text() != "")
    {
        serverArgs << "-p" << ui->portEdit->text();
    }
    if (ui->addressEdit->text() != "")
    {
        serverArgs << "-a" << ui->addressEdit->text();
    }
    if (ui->urlEdit->text() != "")
    {
        serverArgs << "-u" << ui->urlEdit->text();
    }
    if (ui->hostEdit->text() != "")
    {
        serverArgs << "-f" << ui->hostEdit->text();
    }
    if (ui->sourceEdit->toPlainText() != "")
    {
        QString source = ui->sourceEdit->toPlainText();
        QStringList sources = source.split(QRegularExpression("[,.'，。 \n]+"));
        sources.removeAll("");
        serverArgs << "-o" << sources;
    }
    if (ui->strictCheck->isChecked() == true)
    {
        serverArgs << "-s";
    }
    qDebug() << "Server Arguments:" << serverArgs.join(" ").toUtf8().data();
}

void MainWindow::startServer()
{
    getPath();
    getArgs();
    if (serverFile != "")
    {
        server->start(serverFile, serverArgs);
        if (!server->waitForStarted())
        {
            outLog(server->errorString());
        }
    }
    else
    {
        outLog(tr("Server not found."));
    }
}

void MainWindow::outLog(const QString &log)
{
    // avoid sub thread update ui
    QString backLog = log;
    QTimer::singleShot(0, this, [this, backLog]()
                       { ui->outText->append(backLog); });
}

void MainWindow::stopServer()
{
    server->close();
    updateSettings();
    config->writeSettings();
}

void MainWindow::on_tray_activated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        this->show();
        break;
    default:;
    }
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    this->hide();
    e->ignore();
}
