#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "config.h"
#include "server.h"

#include <QCloseEvent>
#include <QMainWindow>

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
    bool setProxy(const bool &enable);
    bool isProxy();

public slots:
    void show(const bool &show);
    void exit();

private:
    Ui::MainWindow *ui;
    Server *server;
    Config *config;

    void setTheme(const QString &theme);
    bool event(QEvent *e);
    void loadSettings();
    void updateSettings();

private slots:
    void on_startup(const bool &enable);
    void on_about();
    void on_aboutQt();
    void on_https(const bool &enable);
    void on_apply();
};
#endif // MAINWINDOW_H
