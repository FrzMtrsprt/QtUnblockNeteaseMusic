#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <QObject>

class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = nullptr);

    QString port;
    QString address;
    QString url;
    QString host;
    QString source;
    bool strict;
    bool startup;

    void readSettings();
    void writeSettings();
    void setStartup(int);

private:
    QSettings *settings;
};

#endif // CONFIG_H
