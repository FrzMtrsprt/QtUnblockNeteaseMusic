#include "envdialog.h"
#include "ui_envdialog.h"

#include <QDesktopServices>

using namespace Qt::Literals::StringLiterals;

EnvDialog::EnvDialog(Config *config, QWidget *parent)
    : QDialog(parent), ui(new Ui::EnvDialog)
{
    ui->setupUi(this);
    ui->tokenEdit->setText(config->token);
    ui->endpointEdit->setText(config->endpoint);
    ui->cnrelayEdit->setText(config->cnrelay);
    ui->otherEdit->setPlainText(config->other.join("\n"));
    ui->envEdit->setPlainText(config->env.join("\n"));

    this->config = config;

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &EnvDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::helpRequested, this, &EnvDialog::help);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &EnvDialog::close);
}

EnvDialog::~EnvDialog()
{
    delete ui;
}

void EnvDialog::accept()
{
    config->token = ui->tokenEdit->text();
    config->endpoint = ui->endpointEdit->text();
    config->cnrelay = ui->cnrelayEdit->text();
    config->other = ui->otherEdit->toPlainText().split('\n', Qt::SkipEmptyParts);
    config->env = ui->envEdit->toPlainText().split('\n', Qt::SkipEmptyParts);
    QDialog::accept();
}

void EnvDialog::help()
{
    QDesktopServices::openUrl(QUrl(
        u"https://github.com/UnblockNeteaseMusic/server#%E7%8E%AF%E5%A2%83%E5%8F%98%E9%87%8F"_s));
}
