#include "config.h"

#include <QApplication>

using namespace Qt::Literals::StringLiterals;

Config::Config() : QSettings(u"config.ini"_s, IniFormat)
{
    beginGroup(QApplication::applicationName());
}

Config::~Config()
{
    endGroup();
}

void Config::readSettings()
{
    port = value("port").toString();
    address = value("address").toString();
    url = value("url").toString();
    host = value("host").toString();
    sources = value("sources").toStringList();
    strict = value("strict").toBool();
    startup = value("startup").toBool();
    theme = value("theme").toString();
}

void Config::writeSettings()
{
    setValue("port", port);
    setValue("address", address);
    setValue("url", url);
    setValue("host", host);
    setValue("sources", sources);
    setValue("strict", strict);
    setValue("startup", startup);
    setValue("theme", theme);
}
