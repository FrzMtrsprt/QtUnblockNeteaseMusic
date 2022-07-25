#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"

#include <QCloseEvent>
#include <QMainWindow>
#include <QProcess>

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
    void on_actionAbout_triggered();
    void on_exitBtn_clicked();
    void on_restartBtn_clicked();
    void on_readoutput();
    void on_readerror();

private:
    Ui::MainWindow *ui;
    QProcess *server = new QProcess();
    QString serverFile;
    QStringList serverArgs;
    Config *config = new Config();
    void closeEvent(QCloseEvent *); // reload closeEvent to add my own actions
    void loadSettings();
    void updateSettings();
    void getPath();
    void getArgs();
    void startServer();
    void outLog(const QString &log);
    void stopServer();
};
#endif // MAINWINDOW_H
