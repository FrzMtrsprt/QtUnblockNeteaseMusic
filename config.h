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
    void readSettings();
    void writeSettings();

private:
    QSettings *settings = new QSettings();
};

#endif // CONFIG_H
