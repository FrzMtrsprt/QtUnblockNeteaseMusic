#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
    QProcess *server = new QProcess();
    QString serverFile;
    QStringList serverArgs;
    void closeEvent(QCloseEvent *);
    void getPath();
    void getArgs();
    void startServer();
    void outLog(const QString &log);
    void stopServer();

private slots:
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_exitBtn_clicked();
    void on_restartBtn_clicked();
    void on_readoutput();
    void on_readerror();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
