#ifndef CONFIG_H
#define CONFIG_H

#include "param.h"

#include <QSettings>

class Config : public QSettings
{
public:
    Config();
    ~Config();

    QHash<const char *, Param> params;

    bool startup;
    QString theme;
    bool debugInfo;

    QStringList other;

    QStringList env;

    void readSettings();
    void writeSettings();
};

#endif // CONFIG_H
