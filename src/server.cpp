#include "server.h"

#include <QDir>
#include <QMessageBox>

#ifdef Q_OS_WIN
#include "utils/winutils.h"
#endif

using namespace Qt::StringLiterals;

Server::Server(Config *config)
    : QProcess(), config(config)
{
    connect(this, &Server::readyReadStandardOutput,
            [this]
            { emit out(readAllStandardOutput()); });
    connect(this, &Server::readyReadStandardError,
            [this]
            { emit err(readAllStandardError()); });
    connect(this, &Server::finished,
            this, &Server::on_finished);
}

Server::~Server()
{
    close();
}

bool Server::findProgram()
{
    QDir appDir = QDir::current();

    QProcess node;
    node.start(u"node"_s, {u"-v"_s}, ReadOnly);
    const bool hasNode = node.waitForFinished() && node.exitCode() == 0;

    // Find server script
    appDir.setFilter(QDir::Dirs);
    appDir.setNameFilters({u"unblock*"_s, u"server*"_s});
    for (const QString &entry : appDir.entryList())
    {
        QDir serverDir(entry);
        if (serverDir.exists(u"app.js"_s))
        {
            if (hasNode)
            {
                program = u"node"_s;
                arguments = {serverDir.filePath(u"app.js"_s)};
                return true;
            }
            else
            {
                emit out(tr("Node.js is not installed."));
                break;
            }
        }
    }

    // Find server binary
    appDir.setFilter(QDir::Files);
#ifdef Q_OS_WIN
    appDir.setNameFilters({u"unblock*.exe"_s});
#else
    appDir.setNameFilters({u"unblock*"_s});
#endif
    for (const QString &entry : appDir.entryList())
    {
        program = entry;
        arguments = {};
        return true;
    }
    return false;
}

void Server::loadArgs()
{
    for (const Param &param : config->params)
    {
        switch (param.typeId)
        {
        case QMetaType::Bool:
            if (param.value<bool>())
            {
                arguments << param.prefix;
            }
            break;
        case QMetaType::QString:
            if (param.value<QString>().size())
            {
                arguments << param.prefix << param.value<QString>();
            }
            break;
        case QMetaType::QStringList:
            if (param.value<QStringList>().size())
            {
                arguments << param.prefix << param.value<QStringList>();
            }
            break;
        }
    }
    for (const QString &entry : config->other)
    {
        arguments << entry.split(u' ');
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    for (const QString &entry : config->env)
    {
        const int pos = entry.indexOf(u'=');
        if (pos > 0 && pos < entry.size() - 1)
        {
            env.insert(entry.sliced(0, pos), entry.sliced(pos + 1));
        }
    }
    if (config->debugInfo && !env.contains(u"LOG_LEVEL"_s))
    {
        env.insert(u"LOG_LEVEL"_s, u"debug"_s);
    }
    setProcessEnvironment(env);
}

void Server::start()
{
    if (state() != NotRunning)
    {
        return;
    }
    if (findProgram())
    {
        loadArgs();
        if (config->debugInfo)
        {
            emit out(program + u' ' + arguments.join(u' '));
        }
        QProcess::start(program, arguments, QIODeviceBase::ReadOnly);
        if (!waitForStarted())
        {
            emit out(errorString());
        }
    }
    else
    {
        emit out(tr("Server not found."));
    }
}

void Server::restart()
{
    disconnect(this, &Server::finished,
               this, &Server::on_finished);
    close();
    connect(this, &Server::finished,
            this, &Server::on_finished);
    start();
}

void Server::on_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Server finished with code" << exitCode;
    qDebug() << "Exit status" << exitStatus;
    if (exitCode != 0)
    {
        emit out(tr("Process exited with code %1.\n"
                    "Please change the arguments or "
                    "check port usage and try again.")
                     .arg(exitCode));
    }
}
