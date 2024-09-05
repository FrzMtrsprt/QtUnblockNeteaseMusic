#pragma once

#include <QSimpleUpdater.h>

class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    UpdateChecker();
    ~UpdateChecker();
    void checkUpdate();
signals:
    void ready(const bool &isNewVersion, const QString &version, const QString &url);

private:
    QSimpleUpdater *m_updater;
};
