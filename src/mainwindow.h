#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"

#include <QCloseEvent>
#include <QMainWindow>
#include <QProcess>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();
    void on_startupCheckBox_stateChanged(const int state);
    void on_actionAbout_triggered();
    void on_actionAboutQt_triggered();
    void on_exitBtn_clicked();
    void on_restartBtn_clicked();
    void on_readoutput();
    void on_readerror();

private:
    Ui::MainWindow *ui;
    QProcess *server;
    QString serverFile;
    QStringList serverArgs;
    Config *config;

    QSystemTrayIcon *tray;
    QMenu *trayMenu;
    QAction *trayExit;
    QAction *trayShow;

    void closeEvent(QCloseEvent *e); // reload closeEvent to add my own actions
    void loadSettings();
    void updateSettings();
    bool getServer();
    void getArgs();
    void startServer();
    void outLog(const QString &log);
    void stopServer();
};
#endif // MAINWINDOW_H
