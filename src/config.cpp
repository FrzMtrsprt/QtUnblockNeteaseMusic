#include "config.h"

#include <QApplication>

using namespace Qt;

Config::Config()
    : QSettings(u"config.ini"_s, IniFormat) {}

Config::~Config() {}

void Config::readSettings()
{
    beginGroup(QApplication::applicationName());

    port = value("port").toString();
    address = value("address").toString();
    url = value("url").toString();
    host = value("host").toString();
    sources = value("sources").toStringList();
    strict = value("strict").toBool();
    startup = value("startup").toBool();
    theme = value("theme").toString();

    endGroup();
}

void Config::writeSettings()
{
    beginGroup(QApplication::applicationName());

    setValue("port", port);
    setValue("address", address);
    setValue("url", url);
    setValue("host", host);
    setValue("sources", sources);
    setValue("strict", strict);
    setValue("startup", startup);
    setValue("theme", theme);

    endGroup();
}
