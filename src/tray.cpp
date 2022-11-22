#include "tray.h"

using namespace Qt;

Tray::Tray() : QSystemTrayIcon()
{
    menu = new QMenu();
    show = new QAction();
    exit = new QAction();

    show->setText(tr("Show"));
    exit->setText(tr("Exit"));

    menu->addAction(show);
    menu->addAction(exit);

    setContextMenu(menu);
    setIcon(QIcon(u":/res/icon.png"_s));
    setToolTip(u"QtUnblockNeteaseMusic"_s);

    connect(this, &Tray::activated, this, &Tray::on_activated);
}

Tray::~Tray()
{
    show->~QAction();
    exit->~QAction();
    menu->~QMenu();
}

void Tray::on_activated(ActivationReason reason)
{
    // emit only when tray icon is left clicked
    if (reason == Trigger)
    {
        emit clicked();
    }
}
