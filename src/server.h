#ifndef SERVER_H
#define SERVER_H

#include "config/config.h"

#include <QProcess>

class Server : public QProcess
{
    Q_OBJECT

public:
    Server(Config *config);
    ~Server();

    void start();
    void restart();

signals:
    void log(const QString &message);
    void logClear();

private:
    Config *config;
    QString program;
    QStringList arguments;

    bool findProgram();
    void loadArgs();
    void on_stdout();
    void on_stderr();
};

#endif // SERVER_H