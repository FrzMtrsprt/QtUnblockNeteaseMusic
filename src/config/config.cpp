#include "config.h"

#include <QApplication>

using namespace Qt::Literals::StringLiterals;

Config::Config() : QSettings(u"config.ini"_s, IniFormat)
{
    params["port"] = Param(u"-p"_s, QMetaType::QString, u"11111:11112"_s);
    params["address"] = Param(u"-a"_s, QMetaType::QString, u"127.0.0.1"_s);
    params["url"] = Param(u"-u"_s, QMetaType::QString);
    params["host"] = Param(u"-f"_s, QMetaType::QString);
    params["sources"] = Param(u"-o"_s, QMetaType::QStringList);
    params["strict"] = Param(u"-s"_s, QMetaType::Bool, true);

    params["token"] = Param(u"-t"_s, QMetaType::QString);
    params["endpoint"] = Param(u"-e"_s, QMetaType::QString);
    params["cnrelay"] = Param(u"-c"_s, QMetaType::QString);

    beginGroup(QApplication::applicationName());
}

Config::~Config()
{
    endGroup();
}

void Config::readSettings()
{
    for (const auto &[key, value] : params.asKeyValueRange())
    {
        QVariant v = QSettings::value(key);
        if (v.isValid())
        {
            v.convert(QMetaType(value.typeId));
            value.setValue(v);
        }
    }

    startup = value("startup").toBool();
    theme = value("theme").toString();
    debugInfo = value("debugInfo").toBool();

    other = value("other").toStringList();

    env = value("env").toStringList();
}

void Config::writeSettings()
{
    for (const auto &[key, value] : params.asKeyValueRange())
    {
        setValue(key, value);
    }

    setValue("startup", startup);
    setValue("theme", theme);
    setValue("debugInfo", debugInfo);

    setValue("other", other);

    setValue("env", env);
}
