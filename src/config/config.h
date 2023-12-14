#pragma once

#include "param.h"

#include <QSettings>

class Config : QSettings
{
public:
    Config();
    ~Config();

    QList<Param> params;

    bool startup;
    bool startMinimized;
    bool checkUpdate;
    QString theme;
    bool debugInfo;

    QStringList other;

    QStringList env;

    void readSettings();
    void writeSettings();
};
