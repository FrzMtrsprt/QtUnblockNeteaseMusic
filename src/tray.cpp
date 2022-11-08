#include "tray.h"

using namespace Qt;

Tray::Tray(QWidget *parent) : QSystemTrayIcon(parent)
{
    menu = new QMenu(parent);
    show = new QAction(menu);
    exit = new QAction(menu);

    show->setText(tr("Show"));
    exit->setText(tr("Exit"));

    menu->addAction(show);
    menu->addAction(exit);

    this->setContextMenu(menu);
    this->setIcon(QIcon(u":/res/icon.png"_s));
    this->setToolTip(u"QtUnblockNeteaseMusic"_s);

    // emit only when tray icon is left clicked
    connect(this, &Tray::activated, this,
            [this](ActivationReason reason)
            {
                if (reason == Trigger)
                {
                    emit clicked();
                }
            });
}

Tray::~Tray()
{
    show->~QAction();
    exit->~QAction();
    menu->~QMenu();
}
