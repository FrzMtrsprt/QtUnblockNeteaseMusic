#ifndef SERVER_H
#define SERVER_H

#include "config/config.h"

#include <QPlainTextEdit>
#include <QProcess>

class Server : public QProcess
{
    Q_OBJECT

public:
    Server(QPlainTextEdit *output, Config *config);
    ~Server();

    void start();
    void restart();

private:
    QPlainTextEdit *output;
    Config *config;
    QString program;
    QStringList arguments;

    bool findProgram();
    void loadArgs();
    void on_stdout();
    void on_stderr();
};

#endif // SERVER_H