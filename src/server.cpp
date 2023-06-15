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
    QDir appDir = QDir::current();

    // Check if node.js installed
    QProcess::start(u"node"_s, {u"-v"_s}, ReadOnly);
    const bool hasNode = waitForStarted();
    close();

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
                output->append(tr("Node.js is not installed."));
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
            if (param.toBool())
            {
                arguments << param.prefix;
            }
            break;
        case QMetaType::QString:
            if (param.toString().size())
            {
                arguments << param.prefix << param.toString();
            }
            break;
        case QMetaType::QStringList:
            if (param.toStringList().size())
            {
                arguments << param.prefix << param.toStringList();
            }
            break;
        }
    }
    if (!config->other.isEmpty())
    {
        for (const QString &entry : config->other)
        {
            arguments << entry.split(u' ');
        }
    }

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    for (const QString &entry : config->env)
    {
        const int pos = entry.indexOf('=');
        if (pos > 0)
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
    if (findProgram())
    {
        loadArgs();
        if (config->debugInfo)
        {
            output->append(program + u' ' + arguments.join(u' '));
        }
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

    QMessageBox *errorDlg = new QMessageBox(output);
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
