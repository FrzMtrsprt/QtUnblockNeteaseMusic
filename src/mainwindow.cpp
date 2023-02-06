#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QCloseEvent>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <QRegularExpression>
#include <QStyle>
#include <QStyleFactory>

#include "config.h"

#ifdef Q_OS_WIN
#include "utils/winutils.h"
#endif

using namespace Qt::Literals::StringLiterals;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // connect MainWindow signals
    connect(ui->actionExit, &QAction::triggered,
            this, &MainWindow::exit);
    connect(ui->actionAbout, &QAction::triggered,
            this, &MainWindow::on_about);
    connect(ui->actionAboutQt, &QAction::triggered,
            this, &MainWindow::on_aboutQt);
    connect(ui->startupCheckBox, &QCheckBox::clicked,
            this, &MainWindow::on_startup);
    connect(ui->proxyCheckBox, &QCheckBox::clicked,
            this, &MainWindow::setProxy);
    connect(ui->applyBtn, &QPushButton::clicked,
            this, &MainWindow::on_apply);
    connect(ui->exitBtn, &QPushButton::clicked,
            this, &MainWindow::exit);

    // setup theme menu
    for (const QString &style : QStyleFactory::keys())
    {
        // reference: https://stackoverflow.com/a/45265455
        QAction *action = ui->menuTheme->addAction(style);
        connect(action, &QAction::triggered,
                this, [this, style]
                { setTheme(style); });
    }

    // setup & load config
    config = new Config();
    loadSettings();

    // setup & start server
    qDebug("---Starting server---");
    server = new QProcess();
    connect(server, &QProcess::readyReadStandardOutput,
            this, &MainWindow::on_stdout);
    connect(server, &QProcess::readyReadStandardError,
            this, &MainWindow::on_stderr);
    startServer();
}

MainWindow::~MainWindow()
{
    delete ui;
    server->~QProcess();
    config->~Config();
}

void MainWindow::setTheme(const QString &theme)
{
    QStyle *style = QStyleFactory::create(theme);
    if (style)
    {
#ifdef Q_OS_WIN
        WinUtils::setWindowFrame(winId(), style);
#endif
        QApplication::setStyle(style);
        if (style->name() == u"windowsvista"_s || style->name() == u"macOS"_s)
        {
            // Do not set palette for native styles
            QApplication::setPalette(QPalette());
        }
        else
        {
            QApplication::setPalette(style->standardPalette());
        }
    }
}

// Reload QWidget::show()
void MainWindow::show(const bool &show)
{
    setVisible(show);
    activateWindow();

#ifdef Q_OS_WIN
    WinUtils::setThrottle(!show);
#endif
}

bool MainWindow::setProxy(const bool &enable)
{
    const QString address = config->address;
    const QString port = config->port.split(':')[0];
    bool ok = false;
#ifdef Q_OS_WIN
    ok = WinUtils::setSystemProxy(enable, address, port);
#endif
    if (!ok)
    {
        ui->proxyCheckBox->setChecked(isProxy());

        const QString title = tr("Error");
        const QString text =
            tr("Failed to set system proxy.\n"
               "Please check the server port "
               "and address, and try again.");

        QMessageBox *errorDlg = new QMessageBox();
        errorDlg->setAttribute(Qt::WA_DeleteOnClose);
        errorDlg->setWindowTitle(title);
        errorDlg->setText(text);
        errorDlg->setIcon(QMessageBox::Warning);
#ifdef Q_OS_WIN
        WinUtils::setWindowFrame(errorDlg->winId(), errorDlg->style());
#endif
        errorDlg->exec();
    }
    return ok;
}

bool MainWindow::isProxy()
{
    const QString address = config->address;
    const QString port = config->port.split(':')[0];
    bool isProxy = false;
#ifdef Q_OS_WIN
    isProxy = WinUtils::isSystemProxy(address, port);
#endif
    return isProxy;
}

void MainWindow::exit()
{
    qDebug("---Shutting down---");
    server->close();
    updateSettings();
    QApplication::exit();
}

void MainWindow::on_about()
{
    const QPixmap logo =
        QPixmap(u":/res/icon.png"_s)
            .scaled(100, 100,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation);

    const QString text =
        tr("<h3>About %1</h3>"
           "<p>Version %2</p>")
            .arg(QApplication::applicationName(),
                 QApplication::applicationVersion());

    const QString info =
        tr("<p>A desktop client for UnblockNeteaseMusic, "
           "made with Qt.</p>"
           "<p>Copyright 2022 %1</p>")
            .arg(QApplication::organizationName());

    QMessageBox *aboutDlg = new QMessageBox(this);
    aboutDlg->setAttribute(Qt::WA_DeleteOnClose);
    aboutDlg->setWindowTitle(tr("About"));
    aboutDlg->setIconPixmap(logo);
    aboutDlg->setText(text);
    aboutDlg->setInformativeText(info);
    aboutDlg->setStandardButtons(QMessageBox::Ok);
    aboutDlg->setEscapeButton(QMessageBox::Ok);
    aboutDlg->addButton(QMessageBox::Help)->setText(u"GitHub"_s);
#ifdef Q_OS_WIN
    WinUtils::setWindowFrame(aboutDlg->winId(), aboutDlg->style());
#endif

    if (aboutDlg->exec() == QMessageBox::Help)
    {
        const QUrl url(QApplication::organizationDomain());
        QDesktopServices::openUrl(url);
    }
}

void MainWindow::on_aboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_startup(const bool &enable)
{
#ifdef Q_OS_WIN
    WinUtils::setStartup(enable);
#endif
}

void MainWindow::on_stdout()
{
    const QByteArray log = server->readAllStandardOutput();
    ui->outText->append(log);
}

void MainWindow::on_stderr()
{
    const QString title = tr("Server error");
    const QString text =
        tr("The UnblockNeteaseMusic server "
           "ran into an error.\n"
           "Please change the arguments or "
           "check port usage and try again.");
    const QByteArray log = server->readAllStandardError();

    QMessageBox *errorDlg = new QMessageBox(this);
    errorDlg->setAttribute(Qt::WA_DeleteOnClose);
    errorDlg->setWindowTitle(title);
    errorDlg->setText(text);
    errorDlg->setDetailedText(log);
    errorDlg->setIcon(QMessageBox::Warning);
#ifdef Q_OS_WIN
    WinUtils::setWindowFrame(errorDlg->winId(), errorDlg->style());
#endif
    errorDlg->exec();
}

void MainWindow::on_apply()
{
    qDebug("---Restarting server---");
    const bool wasProxy = isProxy();
    server->close();
    ui->outText->clear();
    startServer();
    if (wasProxy)
    {
        setProxy(true);
    }
}

void MainWindow::loadSettings()
{
    qDebug("Loading settings");

    // load settings from file into variables
    config->readSettings();

    // load settings from variables into ui
    ui->portEdit->setText(config->port);
    ui->addressEdit->setText(config->address);
    ui->urlEdit->setText(config->url);
    ui->hostEdit->setText(config->host);
    ui->sourceEdit->append(config->sources.join(u", "_s));
    ui->strictCheckBox->setChecked(config->strict);
    ui->startupCheckBox->setChecked(config->startup);
    setTheme(config->theme);

    qDebug("Load settings done");
}

void MainWindow::updateSettings()
{
    qDebug("Updating settings");

    static const QRegularExpression sep(u"\\W+"_s);

    // update settings from ui into variables
    config->port = ui->portEdit->text();
    config->address = ui->addressEdit->text();
    config->url = ui->urlEdit->text();
    config->host = ui->hostEdit->text();
    config->sources = ui->sourceEdit->toPlainText()
                          .split(sep, Qt::SkipEmptyParts);
    config->strict = ui->strictCheckBox->isChecked();
    config->startup = ui->startupCheckBox->isChecked();
    config->theme = QApplication::style()->name();

    // write settings from variables into file
    config->writeSettings();

    qDebug("Update settings done");
}

bool MainWindow::getServer(QString &program, QStringList &arguments)
{
    const QFileInfoList entries =
        QDir::current().entryInfoList(
            {u"unblock*"_s, u"server*"_s});

    for (const QFileInfo &entry : entries)
    {
        const QString entryPath = entry.filePath();

        // server is packaged execuable
        if (entry.isFile())
        {
            program = entryPath;
            arguments = {};
            return true;
        }

        // server is node script
        if (entry.isDir())
        {
            const QString scriptPath =
                entryPath + u"/app.js"_s;

            if (QFileInfo::exists(scriptPath))
            {
                // Check if node.js installed
                server->start(u"node"_s, {u"-v"_s},
                              QIODeviceBase::ReadOnly);
                const bool exists = server->waitForStarted();
                server->close();

                if (exists)
                {
                    program = u"node"_s;
                    arguments = {scriptPath};
                    return true;
                }
                else
                {
                    ui->outText->append(
                        tr("Node.js is not installed."));
                }
            }
        }
    }
    return false;
}

void MainWindow::getArgs(QStringList &arguments)
{
    updateSettings();

    if (!config->port.isEmpty())
    {
        arguments << u"-p"_s << config->port;
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
}

void MainWindow::startServer()
{
    QString program;
    QStringList arguments;

    if (getServer(program, arguments))
    {
        getArgs(arguments);
        qDebug() << "Server program:"
                 << program;
        qDebug() << "Server arguments:"
                 << arguments.join(u" "_s);
        server->start(program, arguments,
                      QIODeviceBase::ReadOnly);
        if (!server->waitForStarted())
        {
            ui->outText->append(server->errorString());
        }
    }
    else
    {
        ui->outText->append(tr("Server not found."));
    }
}

// Event reloads
bool MainWindow::event(QEvent *e)
{
    switch (e->type())
    {
    case QEvent::KeyPress:
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(e);
        if (keyEvent->key() == Qt::Key_Escape)
        {
            hide();
        }
        break;
    }
    case QEvent::Close:
    {
        hide();
        e->ignore();
        return true;
    }
    case QEvent::WindowActivate:
    {
        ui->proxyCheckBox->setChecked(isProxy());
        break;
    }
    };
    return QMainWindow::event(e);
}
