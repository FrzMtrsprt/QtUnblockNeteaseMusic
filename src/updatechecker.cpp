#include "updatechecker.h"

#include "version.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QTimer>

UpdateChecker::UpdateChecker()
{
    manager = new QNetworkAccessManager();
    connect(manager, &QNetworkAccessManager::finished, this, &UpdateChecker::parseReply);
}

UpdateChecker::~UpdateChecker()
{
    delete manager;
}

void UpdateChecker::checkUpdate()
{
    const QNetworkRequest request(QUrl(PROJECT_API_URL));
    manager->get(request);

    // Check again in 24 hours
    QTimer::singleShot(24 * 60 * 60 * 1000, this, &UpdateChecker::checkUpdate);
}

void UpdateChecker::parseReply(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << reply->errorString();
        return;
    }

    const QByteArray data = reply->readAll();
    reply->deleteLater();
    const QJsonDocument doc = QJsonDocument::fromJson(data);
    const QJsonObject obj = doc.object();
    const QString tagName = obj["tag_name"].toString();
    // Split the version string into three parts
    const QString version = tagName.sliced(1);
    qDebug() << "Latest version:" << version;
    emit ready(isNewVersion(version, PROJECT_VERSION), version);
}

bool UpdateChecker::isNewVersion(const QString &target, const QString &current)
{
    const QStringList targetParts = target.split(".");
    const QStringList currentParts = current.split(".");
    for (int i = 0; i < targetParts.size() && i < currentParts.size(); i++)
    {
        if (targetParts[i].toInt() > currentParts[i].toInt())
        {
            return true;
        }
        if (targetParts[i].toInt() < currentParts[i].toInt())
        {
            return false;
        }
    }
    return false;
}