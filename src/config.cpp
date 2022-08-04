#include "config.h"

#include <QSettings>

Config::Config(QObject *parent)
    : QObject{parent}
{
    settings = new QSettings("./config.ini", QSettings::IniFormat);
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
