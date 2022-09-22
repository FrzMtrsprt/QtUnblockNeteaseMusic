#include "config.h"

#include <QApplication>

Config::Config(QObject *parent) : QObject{parent}
{
    settings = new QSettings(QApplication::applicationDirPath() + "/config.ini",
                             QSettings::IniFormat);
}

void Config::readSettings()
{
    settings->beginGroup(QApplication::applicationName());
    port = settings->value("port").toString();
    address = settings->value("address").toString();
    url = settings->value("url").toString();
    host = settings->value("host").toString();
    sources = settings->value("sources").toStringList();
    strict = settings->value("strict").toBool();
    startup = settings->value("startup").toBool();
    theme = settings->value("theme").toString();
    settings->endGroup();
}

void Config::writeSettings()
{
    settings->beginGroup(QApplication::applicationName());
    settings->setValue("port", port);
    settings->setValue("address", address);
    settings->setValue("url", url);
    settings->setValue("host", host);
    settings->setValue("sources", sources);
    settings->setValue("strict", strict);
    settings->setValue("startup", startup);
    settings->setValue("theme", theme);
    settings->endGroup();
}
