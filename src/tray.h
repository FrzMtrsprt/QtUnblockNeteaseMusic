#ifndef TRAY_H
#define TRAY_H

#include <QMenu>
#include <QSystemTrayIcon>

class Tray : public QSystemTrayIcon
{
    Q_OBJECT

public:
    Tray(QWidget *parent = nullptr);
    QAction *show;
    QAction *exit;

private:
    QMenu *menu;
};

#endif // TRAY_H
