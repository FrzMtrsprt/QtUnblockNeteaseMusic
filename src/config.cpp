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
    httpPort = value("httpPort").toString();
    httpsPort = value("httpsPort").toString();
    useHttps = value("useHttps").toBool();
    address = value("address").toString();
    url = value("url").toString();
    host = value("host").toString();
    sources = value("sources").toStringList();
    strict = value("strict").toBool();
    startup = value("startup").toBool();
    theme = value("theme").toString();
    debugInfo = value("debugInfo").toBool();
    env = value("env").toStringList();
}

void Config::writeSettings()
{
    setValue("httpPort", httpPort);
    setValue("httpsPort", httpsPort);
    setValue("useHttps", useHttps);
    setValue("address", address);
    setValue("url", url);
    setValue("host", host);
    setValue("sources", sources);
    setValue("strict", strict);
    setValue("startup", startup);
    setValue("theme", theme);
    setValue("debugInfo", debugInfo);
    setValue("env", env);
}
