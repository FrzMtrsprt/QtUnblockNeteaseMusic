#ifndef TRAY_H
#define TRAY_H

#include <QMenu>
#include <QSystemTrayIcon>

class Tray : public QSystemTrayIcon
{
    Q_OBJECT

public:
    Tray();
    ~Tray();
    QAction *show;
    QAction *exit;

signals:
    void clicked();

private:
    QMenu *menu;

private slots:
    void on_activated(ActivationReason reason);
};

#endif // TRAY_H
