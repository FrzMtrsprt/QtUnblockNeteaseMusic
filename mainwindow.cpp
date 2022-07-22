#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QCloseEvent>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(server , SIGNAL(readyReadStandardOutput()) , this , SLOT(on_readoutput()));
    connect(server , SIGNAL(readyReadStandardError()) , this , SLOT(on_readerror()));
    QObject::connect(server, &QProcess::finished, [this]()
                     {outLog(tr(server->readAllStandardOutput()));});
    startServer();
}

void MainWindow::on_readoutput()
{
    ui->outText->append(server->readAllStandardOutput().data());   //将输出信息读取到编辑框
}

void MainWindow::on_readerror()
{
    QMessageBox::information(0, "Error", server->readAllStandardError().data());    //弹出信息框提示错误信息
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
    QString dlgTitle = "About";
    QString strInfo = "This is an about dialog.";
    QMessageBox::about(this, dlgTitle, strInfo);
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

void MainWindow::getPath()
{
    QDir serverDir(QCoreApplication::applicationDirPath());
    serverDir.setNameFilters({"unblock*"});
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
    }
    else
    {
        serverFile = NULL;
    }
    qDebug() << "Server File:" << serverFile.toUtf8().data();
}

void MainWindow::getArgs()
{
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
        source.replace(",", " ");
        source.replace("\n", " ");
        serverArgs << "-o" << source;
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
    server->start(serverFile, serverArgs);
    if (server->waitForStarted())
    {
        outLog(serverFile + " " + serverArgs.join(" ").toUtf8().data());
    }
    else
    {
        outLog(server->errorString());
        return;
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
    server->closeReadChannel(QProcess::StandardOutput);
    server->close();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    stopServer();
}
