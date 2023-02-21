#include "server.h"

#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QMessageBox>

#ifdef Q_OS_WIN
#include "utils/winutils.h"
#endif

using namespace Qt::Literals::StringLiterals;

Server::Server(QTextEdit *output, Config *config) : QProcess()
{
    this->output = output;
    this->config = config;
    connect(this, &Server::readyReadStandardOutput,
            this, &Server::on_stdout);
    connect(this, &Server::readyReadStandardError,
            this, &Server::on_stderr);
}

Server::~Server()
{
    close();
}

bool Server::findProgram()
{
    const QFileInfoList entries =
        QDir::current().entryInfoList({u"unblock*"_s, u"server*"_s});

    for (const QFileInfo &entry : entries)
    {
        const QString entryPath = entry.filePath();

        // server is packaged execuable
        if (entry.isFile())
        {
            program = entryPath;
            return true;
        }

        // server is node script
        if (entry.isDir())
        {
            const QString scriptPath = entryPath + u"/app.js"_s;

            if (QFileInfo::exists(scriptPath))
            {
                // Check if node.js installed
                QProcess::start(u"node"_s, {u"-v"_s}, ReadOnly);
                const bool exists = waitForStarted();
                close();

                if (exists)
                {
                    program = u"node"_s;
                    arguments = {scriptPath};
                    return true;
                }
                else
                {
                    output->append(tr("Node.js is not installed."));
                }
            }
        }
    }
    return false;
}

void Server::loadArgs()
{
    if (!config->httpPort.isEmpty())
    {
        config->useHttps
            ? arguments << u"-p"_s << config->httpPort + ':' + config->httpsPort
            : arguments << u"-p"_s << config->httpPort;
    }
    if (!config->address.isEmpty())
    {
        arguments << u"-a"_s << config->address;
    }
    if (!config->url.isEmpty())
    {
        arguments << u"-u"_s << config->url;
    }
    if (!config->host.isEmpty())
    {
        arguments << u"-f"_s << config->host;
    }
    if (!config->sources.isEmpty())
    {
        arguments << u"-o"_s << config->sources;
    }
    if (config->strict)
    {
        arguments << u"-s"_s;
    }
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    for (const QString &entry : config->env)
    {
        const QStringList pair = entry.split('=');
        if (pair.size() == 2)
        {
            env.insert(pair[0], pair[1]);
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
    if (findProgram())
    {
        loadArgs();
        QProcess::start(program, arguments, QIODeviceBase::ReadOnly);
        if (!waitForStarted())
        {
            output->append(errorString());
        }
    }
    else
    {
        output->append(tr("Server not found."));
    }
}

void Server::restart()
{
    close();
    output->clear();
    start();
}

void Server::on_stdout()
{
    output->append(readAllStandardOutput());
}

void Server::on_stderr()
{
    const QString title = tr("Server error");
    const QString text =
        tr("The UnblockNeteaseMusic server "
           "ran into an error.\n"
           "Please change the arguments or "
           "check port usage and try again.");

    QMessageBox *errorDlg = new QMessageBox();
    errorDlg->setAttribute(Qt::WA_DeleteOnClose);
    errorDlg->setWindowTitle(title);
    errorDlg->setText(text);
    errorDlg->setDetailedText(readAllStandardError());
    errorDlg->setIcon(QMessageBox::Warning);
#ifdef Q_OS_WIN
    WinUtils::setWindowFrame(errorDlg->winId(), errorDlg->style());
#endif
    errorDlg->exec();
}
