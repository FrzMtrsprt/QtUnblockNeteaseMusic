#pragma once

#include "config/config.h"
#include "updatechecker.h"

#include <QDialog>

namespace Ui
{
    class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(Config *config, QWidget *parent = nullptr);
    ~ConfigDialog();

private:
    Ui::ConfigDialog *ui;
    Config *config;
    UpdateChecker *updateChecker;
    void showUpdateMessage(const bool &isNewVersion, const QString &version);

private slots:
    void accept();
    void help();
};
