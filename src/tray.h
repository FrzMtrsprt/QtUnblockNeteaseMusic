#pragma once

#include "mainwindow.h"

#include <QSystemTrayIcon>

class Tray : public QSystemTrayIcon
{
    Q_OBJECT

public:
    Tray(MainWindow *w);
    ~Tray();
    QAction *show;
    QAction *exit;
    QAction *proxy;

private:
    MainWindow *w;
    QMenu *menu;

private slots:
    void on_activated(ActivationReason reason);
    void on_show();
    void on_proxy(const bool &checked);
    void on_exit();
};
