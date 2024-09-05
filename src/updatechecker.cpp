#include "updatechecker.h"

#include "version.h"

#include <QDebug>
#include <QTimer>

UpdateChecker::UpdateChecker()
{
    m_updater = QSimpleUpdater::getInstance();
    m_updater->setModuleVersion(PROJECT_DEFS_URL, PROJECT_VERSION);
    m_updater->setNotifyOnFinish(PROJECT_DEFS_URL, false);
    m_updater->setNotifyOnUpdate(PROJECT_DEFS_URL, false);

    connect(m_updater, &QSimpleUpdater::checkingFinished, this, [this](const QString &url)
            {
        const bool isNewVersion = m_updater->getUpdateAvailable(url);
        const QString version = m_updater->getLatestVersion(url);
        const QString openUrl = m_updater->getOpenUrl(url);
        emit ready(isNewVersion, version, openUrl); 
        qDebug() << "isNewVersion:" << isNewVersion << version; });
}

UpdateChecker::~UpdateChecker()
{
}

void UpdateChecker::checkUpdate()
{
    m_updater->checkForUpdates(PROJECT_DEFS_URL);

    // Check again in 24 hours
    QTimer::singleShot(24 * 60 * 60 * 1000, this, &UpdateChecker::checkUpdate);
}
