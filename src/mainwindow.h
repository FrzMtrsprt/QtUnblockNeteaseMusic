#pragma once

#include "config/config.h"
#include "server.h"

#include <QLabel>
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
    MainWindow(Config *config);
    ~MainWindow();
    bool setProxy(const bool &enable);
    bool isProxy();
    void gotUpdateStatus(const bool &isNewVersion, const QString &version, const QString &openUrl);

public slots:
    void exit();
    void on_serverOut(const QString &message);
    void on_serverErr(const QString &message);

signals:
    void serverRestart();
    void serverClose();

private:
    Ui::MainWindow *ui;
    Server *server;
    Config *config;
    QLabel *statusLabel;

    void setTheme(const QString &theme);
    bool event(QEvent *e);
    void loadSettings();
    void updateSettings();
    void applySettings();

private slots:
    void on_installCA();
    void on_env();
    void on_about();
    void on_aboutQt();
    void on_apply();
    void on_strictChanged(Qt::CheckState state);
};
