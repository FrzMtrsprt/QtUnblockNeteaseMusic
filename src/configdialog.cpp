#include "configdialog.h"
#include "ui_configdialog.h"

#include <QDesktopServices>

using namespace Qt::Literals::StringLiterals;

ConfigDialog::ConfigDialog(Config *config, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::ConfigDialog),
      config(config)
{
    ui->setupUi(this);
    ui->tokenEdit->setText(config->params[Param::Token].value<QString>());
    ui->endpointEdit->setText(config->params[Param::Endpoint].value<QString>());
    ui->cnrelayEdit->setText(config->params[Param::Cnrelay].value<QString>());
    ui->otherEdit->setPlainText(config->other.join("\n"));
    ui->envEdit->setPlainText(config->env.join("\n"));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ConfigDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::helpRequested, this, &ConfigDialog::help);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ConfigDialog::close);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::accept()
{
    config->params[Param::Token].setValue(ui->tokenEdit->text());
    config->params[Param::Endpoint].setValue(ui->endpointEdit->text());
    config->params[Param::Cnrelay].setValue(ui->cnrelayEdit->text());
    config->other = ui->otherEdit->toPlainText().split('\n', Qt::SkipEmptyParts);
    config->env = ui->envEdit->toPlainText().split('\n', Qt::SkipEmptyParts);
    QDialog::accept();
}

void ConfigDialog::help()
{
    switch (ui->tabWidget->currentIndex())
    {
    case 0:
        QDesktopServices::openUrl(QUrl(
            u"https://github.com/UnblockNeteaseMusic/server#%E9%85%8D%E7%BD%AE%E5%8F%82%E6%95%B0"_s));
        break;
    case 1:
        QDesktopServices::openUrl(QUrl(
            u"https://github.com/UnblockNeteaseMusic/server#%E7%8E%AF%E5%A2%83%E5%8F%98%E9%87%8F"_s));
        break;
    }
}
