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
    void on_show();
    void on_close();
    void on_exit();
    void on_startup(const int &state);
    void on_about();
    void on_aboutQt();
    void on_apply();
    void on_stdout();
    void on_stderr();

private:
    Ui::MainWindow *ui;
    QProcess *server;
    Config *config;

    void setTheme(const QString &theme);
    void closeEvent(QCloseEvent *e); // reload closeEvent to add my own actions
    void loadSettings();
    void updateSettings();
    bool getServer(QString &program, QStringList &argumets);
    void getArgs(QStringList &arguments);
    void startServer();
};
#endif // MAINWINDOW_H
