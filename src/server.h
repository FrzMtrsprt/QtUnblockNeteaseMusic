#pragma once

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

private:
    Config *config;
    QString program;
    QStringList arguments;

    bool findProgram();
    void loadArgs();
    void on_stdout();
    void on_stderr();
    void on_finished(int exitCode, QProcess::ExitStatus exitStatus);
};
