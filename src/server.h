#ifndef SERVER_H
#define SERVER_H

#include "config.h"

#include <QProcess>
#include <QString>
#include <QStringList>
#include <QTextEdit>

class Server : public QProcess
{
    Q_OBJECT

public:
    Server(QTextEdit *output, Config *config);
    ~Server();

    void start();
    void restart();

private:
    QTextEdit *output;
    Config *config;
    QString program;
    QStringList arguments;

    bool findProgram();
    void loadArgs();
    void on_stdout();
    void on_stderr();
};

#endif // SERVER_H