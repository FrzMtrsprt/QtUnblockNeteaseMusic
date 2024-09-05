#include "configdialog.h"
#include "ui_configdialog.h"

#include "version.h"

#include <QDesktopServices>
#include <QLocale>
#include <QMessageBox>

using namespace Qt::StringLiterals;

ConfigDialog::ConfigDialog(Config *config, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::ConfigDialog),
      updateChecker(new UpdateChecker),
      config(config)
{
    ui->setupUi(this);
    ui->startupCheckBox->setChecked(config->startup);
    ui->minimizeCheckBox->setChecked(config->startMinimized);
    ui->updateCheckBox->setChecked(config->checkUpdate);

    ui->tokenEdit->setText(config->params[Param::Token].value<QString>());
    ui->endpointEdit->setText(config->params[Param::Endpoint].value<QString>());
    ui->cnrelayEdit->setText(config->params[Param::Cnrelay].value<QString>());
    ui->otherEdit->setPlainText(config->other.join("\n"));
    ui->envEdit->setPlainText(config->env.join("\n"));

    connect(ui->updateButton, &QPushButton::clicked, updateChecker, &UpdateChecker::checkUpdate);
    connect(updateChecker, &UpdateChecker::ready, this, &ConfigDialog::showUpdateMessage);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ConfigDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::helpRequested, this, &ConfigDialog::help);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ConfigDialog::close);
}

ConfigDialog::~ConfigDialog()
{
    delete updateChecker;
    delete ui;
}

void ConfigDialog::showUpdateMessage(const bool &isNewVersion, const QString &version, const QString &openUrl)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(tr("Update"));
    msgBox.setIcon(QMessageBox::Icon::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setEscapeButton(QMessageBox::Ok);
    if (isNewVersion)
    {
        msgBox.setText(tr("New version %1 is available.").arg(version));
        msgBox.addButton(QMessageBox::Help)->setText(u"View"_s);
    }
    else
    {
        msgBox.setText(tr("You are using the latest version."));
    }

    if (msgBox.exec() == QMessageBox::Help)
    {
        QDesktopServices::openUrl(QUrl(openUrl));
    }
}

void ConfigDialog::accept()
{
    config->startup = ui->startupCheckBox->isChecked();
    config->startMinimized = ui->minimizeCheckBox->isChecked();
    config->checkUpdate = ui->updateCheckBox->isChecked();

    config->params[Param::Token].setValue(ui->tokenEdit->text());
    config->params[Param::Endpoint].setValue(ui->endpointEdit->text());
    config->params[Param::Cnrelay].setValue(ui->cnrelayEdit->text());
    config->other = ui->otherEdit->toPlainText().split(u'\n', Qt::SkipEmptyParts);
    config->env = ui->envEdit->toPlainText().split(u'\n', Qt::SkipEmptyParts);
    QDialog::accept();
}

void ConfigDialog::help()
{
    QUrl url;
    switch (ui->tabWidget->currentIndex())
    {
    case 0:
        url.setUrl(QLocale::system().language() == QLocale::Language::Chinese
                       ? u"https://github.com/FrzMtrsprt/QtUnblockNeteaseMusic/blob/main/README.md"_s
                       : u"https://github.com/FrzMtrsprt/QtUnblockNeteaseMusic/blob/main/README_en.md"_s);
        break;
    case 1:
        url.setUrl(u"https://github.com/UnblockNeteaseMusic/server#%E9%85%8D%E7%BD%AE%E5%8F%82%E6%95%B0"_s);
        break;
    case 2:
        url.setUrl(u"https://github.com/UnblockNeteaseMusic/server#%E7%8E%AF%E5%A2%83%E5%8F%98%E9%87%8F"_s);
        break;
    }
    QDesktopServices::openUrl(url);
}
