#ifndef ENVWINDOW_H
#define ENVWINDOW_H

#include "config/config.h"

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

private slots:
    void accept();
    void help();
};

#endif // ENVWINDOW_H
