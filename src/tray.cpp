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
}