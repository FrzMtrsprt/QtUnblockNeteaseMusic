#include "config.h"

#include <QApplication>

using namespace Qt::StringLiterals;

Config::Config() : QSettings(u"config.ini"_s, IniFormat)
{
    params.emplace(Param::Port, u"port"_s, u"-p"_s, QMetaType::QString, u"11111:11112"_s);
    params.emplace(Param::Address, u"address"_s, u"-a"_s, QMetaType::QString, u"127.0.0.1"_s);
    params.emplace(Param::Url, u"url"_s, u"-u"_s, QMetaType::QString);
    params.emplace(Param::Host, u"host"_s, u"-f"_s, QMetaType::QString);
    params.emplace(Param::Sources, u"sources"_s, u"-o"_s, QMetaType::QStringList);
    params.emplace(Param::Strict, u"strict"_s, u"-s"_s, QMetaType::Bool, true);

    params.emplace(Param::Token, u"token"_s, u"-t"_s, QMetaType::QString);
    params.emplace(Param::Endpoint, u"endpoint"_s, u"-e"_s, QMetaType::QString);
    params.emplace(Param::Cnrelay, u"cnrelay"_s, u"-c"_s, QMetaType::QString);

    beginGroup(QApplication::applicationName());
}

Config::~Config()
{
    endGroup();
}

void Config::readSettings()
{
    for (Param &param : params)
    {
        QVariant v = value(param.name);
        if (v.isValid())
        {
            v.convert(QMetaType(param.typeId));
            param.setValue(v);
        }
    }

    startup = value("startup").value<bool>();
    startMinimized = value("startMinimized").value<bool>();
    theme = value("theme").value<QString>();
    debugInfo = value("debugInfo").value<bool>();

    other = value("other").value<QStringList>();

    env = value("env").value<QStringList>();
}

void Config::writeSettings()
{
    for (const Param &param : params)
    {
        setValue(param.name, param);
    }
    setValue("startup", startup);
    setValue("startMinimized", startMinimized);
    setValue("theme", theme);
    setValue("debugInfo", debugInfo);

    setValue("other", other);

    setValue("env", env);
}
