#ifndef CONFIG_H
#define CONFIG_H

#include "param.h"

#include <QSettings>

class Config : QSettings
{
public:
    Config();
    ~Config();

    QList<Param> params;

    bool startup;
    QString theme;
    bool debugInfo;

    QStringList other;

    QStringList env;

    void readSettings();
    void writeSettings();
};

#endif // CONFIG_H
