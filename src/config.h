#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>

class Config : public QSettings
{
public:
    Config();
    ~Config();

    QString port;
    QString address;
    QString url;
    QString host;
    QStringList sources;
    bool strict;
    bool startup;
    QString theme;

    void readSettings();
    void writeSettings();
};

#endif // CONFIG_H
