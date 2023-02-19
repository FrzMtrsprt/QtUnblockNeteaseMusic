#include "tray.h"

using namespace Qt::Literals::StringLiterals;

Tray::Tray(MainWindow *w) : QSystemTrayIcon(w)
{
    this->w = w;

    menu = new QMenu();
    show = new QAction();
    proxy = new QAction();
    exit = new QAction();

    show->setText(tr("Show"));
    proxy->setText(tr("System Proxy"));
    proxy->setCheckable(true);
    exit->setText(tr("Exit"));

    menu->addAction(show);
    menu->addAction(proxy);
    menu->addAction(exit);

    setContextMenu(menu);
    setIcon(QIcon(u":/res/icon.png"_s));
    setToolTip(u"QtUnblockNeteaseMusic"_s);

    connect(this, &Tray::activated, this, &Tray::on_activated);
    connect(show, &QAction::triggered, this, &Tray::on_show);
    connect(proxy, &QAction::triggered, this, &Tray::on_proxy);
    connect(exit, &QAction::triggered, this, &Tray::on_exit);
}

Tray::~Tray()
{
    show->~QAction();
    proxy->~QAction();
    exit->~QAction();
    menu->~QMenu();
}

void Tray::on_activated(ActivationReason reason)
{
    switch (reason)
    {
    case Context:
        proxy->setChecked(w->isProxy());
        break;
    case Trigger:
        w->show(w->isHidden());
        break;
    default:
        break;
    }
}

void Tray::on_show()
{
    w->show(true);
}

void Tray::on_proxy(const bool &checked)
{
    w->setProxy(checked);
}

void Tray::on_exit()
{
    w->exit();
}
