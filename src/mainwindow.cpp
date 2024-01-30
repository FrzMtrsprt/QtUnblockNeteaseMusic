#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "configdialog.h"
#include "version.h"
#include "wizardpages.h"

#include <QCloseEvent>
#include <QDesktopServices>
#include <QFontDatabase>
#include <QMessageBox>
#include <QRegularExpression>
#include <QStyle>
#include <QStyleFactory>

#ifdef Q_OS_WIN
#include "utils/winutils.h"
#endif

using namespace Qt::StringLiterals;

MainWindow::MainWindow(Config *config)
    : QMainWindow(), ui(new Ui::MainWindow),
      config(config), statusLabel(new QLabel)
{
    ui->setupUi(this);
#ifdef Q_OS_WIN
    QFont font = QFont(u"Consolas"_s);
    font.setStyleHint(QFont::TypeWriter);
#else
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
#endif
    ui->outText->setFont(font);

    // connect MainWindow signals
    connect(ui->actionInstallCA, &QAction::triggered,
            this, &MainWindow::on_installCA);
    connect(ui->actionEnv, &QAction::triggered,
            this, &MainWindow::on_env);
    connect(ui->actionExit, &QAction::triggered,
            this, &MainWindow::exit);
    connect(ui->actionAbout, &QAction::triggered,
            this, &MainWindow::on_about);
    connect(ui->actionAboutQt, &QAction::triggered,
            this, &MainWindow::on_aboutQt);
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

    loadSettings();
    applySettings();
}

MainWindow::~MainWindow()
{
    delete ui;
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
        style->name() == u"windowsvista"_s || style->name() == u"macOS"_s
            // Do not set palette for native styles
            ? QApplication::setPalette(QPalette())
            : QApplication::setPalette(style->standardPalette());
    }
}

bool MainWindow::setProxy(const bool &enable)
{
    const QString address = config->params[Param::Address].value<QString>();
    const QString port = config->params[Param::Port].value<QString>().split(u':')[0];
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

        QMessageBox *errorDlg = new QMessageBox(this);
        errorDlg->setAttribute(Qt::WA_DeleteOnClose);
        errorDlg->setWindowTitle(title);
        errorDlg->setText(text);
        errorDlg->setIcon(QMessageBox::Warning);
        errorDlg->open();
    }
    return ok;
}

bool MainWindow::isProxy()
{
    const QString address = config->params[Param::Address].value<QString>();
    const QString port = config->params[Param::Port].value<QString>().split(u':')[0];
    bool isProxy = false;
#ifdef Q_OS_WIN
    isProxy = WinUtils::isSystemProxy(address + u':' + port);
#endif
    return isProxy;
}

void MainWindow::showVersionStatus(const QString &version)
{
    statusLabel->setText(tr("<a href=\"%1\">New version %2 is available.</a>")
                             .arg(PROJECT_RELEASE_URL)
                             .arg(version));
    statusLabel->setOpenExternalLinks(true);
    ui->statusBar->addWidget(statusLabel);
}

void MainWindow::exit()
{
    qDebug("---Shutting down---");
    emit serverClose();
    updateSettings();
    QApplication::exit();
}

void MainWindow::log(const QString &message)
{
    ui->outText->appendPlainText(message);
}

void MainWindow::logClear()
{
    ui->outText->clear();
}

void MainWindow::on_installCA()
{
#ifdef Q_OS_WIN
    if (!WinUtils::isAdmin())
    {
        QMessageBox::warning(this, tr("Access denied"),
                             tr("Please run QtUnblockNeteaseMusic as "
                                "Administrator from context menu."));
        return;
    }
#endif

    QWizard *wizard = new QWizard(this);
    wizard->addPage(new WizardPage1(wizard));
    wizard->addPage(new WizardPage2(wizard));
    wizard->addPage(new WizardPage3(wizard));

    wizard->setAttribute(Qt::WA_DeleteOnClose);
    wizard->setPixmap(QWizard::LogoPixmap,
                      style()->standardIcon(QStyle::SP_FileIcon).pixmap(64, 64));
    wizard->setWindowTitle(tr("Install certificate"));
    wizard->setWizardStyle(QWizard::ModernStyle);

    wizard->open();
}

void MainWindow::on_env()
{
    ConfigDialog *configDlg = new ConfigDialog(config, this);
    configDlg->setAttribute(Qt::WA_DeleteOnClose);
    configDlg->setFixedSize(configDlg->sizeHint());
    if (configDlg->exec() == QDialog::Accepted)
    {
        updateSettings();
        applySettings();
        emit serverRestart();
    }
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
           "<p>Copyright 2023 %1</p>")
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

void MainWindow::on_apply()
{
    qDebug("---Restarting server---");
    const bool wasProxy = isProxy();
    updateSettings();
    emit serverRestart();
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
    const QStringList split = config->params[Param::Port].value<QString>().split(u':');
    ui->httpEdit->setText(split[0]);
    ui->httpsEdit->setText(split.length() > 1 ? split[1] : u""_s);
    ui->addressEdit->setText(config->params[Param::Address].value<QString>());
    ui->urlEdit->setText(config->params[Param::Url].value<QString>());
    ui->hostEdit->setText(config->params[Param::Host].value<QString>());
    ui->sourceEdit->append(config->params[Param::Sources].value<QStringList>().join(u", "_s));
    ui->strictCheckBox->setChecked(config->params[Param::Strict].value<bool>());
    ui->debugCheckBox->setChecked(config->debugInfo);
    setTheme(config->theme);

    qDebug("Load settings done");
}

void MainWindow::updateSettings()
{
    qDebug("Updating settings");

    static const QRegularExpression sep(u"\\W+"_s);

    // update settings from ui into variables
    const QString port = ui->httpsEdit->text().size()
                             ? ui->httpEdit->text() + u':' + ui->httpsEdit->text()
                             : ui->httpEdit->text();
    config->params[Param::Port].setValue(port);
    config->params[Param::Address].setValue(ui->addressEdit->text());
    config->params[Param::Url].setValue(ui->urlEdit->text());
    config->params[Param::Host].setValue(ui->hostEdit->text());
    config->params[Param::Sources].setValue(ui->sourceEdit->toPlainText().split(sep, Qt::SkipEmptyParts));
    config->params[Param::Strict].setValue(ui->strictCheckBox->isChecked());
    config->debugInfo = ui->debugCheckBox->isChecked();
    config->theme = QApplication::style()->name();

    // write settings from variables into file
    config->writeSettings();

    qDebug("Update settings done");
}

void MainWindow::applySettings()
{
#ifdef Q_OS_WIN
    WinUtils::setStartup(config->startup,
                         config->startMinimized);
#endif
}

// Event reloads
bool MainWindow::event(QEvent *e)
{
    switch (e->type())
    {
    // Hide window on escape key
    case QEvent::KeyPress:
    {
        if (static_cast<QKeyEvent *>(e)->key() == Qt::Key_Escape)
        {
            hide();
        }
        break;
    }

    // Set window border on show
    case QEvent::Show:
    {
#ifdef Q_OS_WIN
        WinUtils::setWindowFrame(winId(), style());
        WinUtils::setThrottle(false);
#endif
        break;
    }

    // Close all child dialogs on close
    case QEvent::Close:
    {
        for (QDialog *dialog : findChildren<QDialog *>())
        {
            dialog->close();
        }
#ifdef Q_OS_WIN
        WinUtils::setThrottle(true);
#endif
        break;
    }

    // Update proxy checkbox on window activation
    case QEvent::WindowActivate:
    {
        ui->proxyCheckBox->setChecked(isProxy());
        break;
    }

    // Set window border for child dialogs
    case QEvent::ChildAdded:
    {
#ifdef Q_OS_WIN
        QObject *object = static_cast<QChildEvent *>(e)->child();
        if (object->isWidgetType())
        {
            QWidget *widget = static_cast<QWidget *>(object);
            if (widget->isWindow())
            {
                WinUtils::setWindowFrame(widget->winId(), widget->style());
            }
        }
#endif
        break;
    }

    default:
        break;
    };
    return QMainWindow::event(e);
}
