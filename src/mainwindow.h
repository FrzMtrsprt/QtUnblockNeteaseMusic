#pragma once

#include "config/config.h"
#include "server.h"

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
    MainWindow(Config *config, Server *server);
    ~MainWindow();
    bool setProxy(const bool &enable);
    bool isProxy();

public slots:
    void exit();
    void log(const QString &message);
    void logClear();

private:
    Ui::MainWindow *ui;
    Server *server;
    Config *config;

    void setTheme(const QString &theme);
    bool event(QEvent *e);
    void loadSettings();
    void updateSettings();

private slots:
    void on_installCA();
    void on_env();
    void on_startup(const bool &enable);
    void on_about();
    void on_aboutQt();
    void on_apply();
};
