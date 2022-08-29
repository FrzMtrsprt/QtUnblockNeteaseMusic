#define AUTO_RUN "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

#include "config.h"

#include <QApplication>

Config::Config(QObject *parent) : QObject{parent}
{
    settings = new QSettings(QApplication::applicationDirPath() + "/config.ini",
                             QSettings::IniFormat);
}

void Config::readSettings()
{
    port = settings->value("port").toString();
    address = settings->value("address").toString();
    url = settings->value("url").toString();
    host = settings->value("host").toString();
    source = settings->value("source").toString();
    strict = settings->value("strict").toBool();
    startup = settings->value("startup").toBool();
}

void Config::writeSettings()
{
    settings->setValue("port", port);
    settings->setValue("address", address);
    settings->setValue("url", url);
    settings->setValue("host", host);
    settings->setValue("source", source);
    settings->setValue("strict", strict);
    settings->setValue("startup", startup);
}

void Config::setStartup(const int &state)
{
    const QString appName = QApplication::applicationName();
    QSettings *registry = new QSettings(AUTO_RUN, QSettings::NativeFormat);
    if (state)
    {
        QString appPath = QApplication::applicationFilePath().replace("/", "\\");
        registry->setValue(appName, "\"" + appPath + "\" -silent");
    }
    else
    {
        registry->remove(appName);
    }
    registry->~QSettings();
}
