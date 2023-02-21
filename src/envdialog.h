#ifndef ENVWINDOW_H
#define ENVWINDOW_H

#include "config.h"

#include <QDialog>

namespace Ui
{
    class EnvDialog;
}

class EnvDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EnvDialog(Config *config, QWidget *parent = nullptr);
    ~EnvDialog();

private:
    Ui::EnvDialog *ui;
    Config *config;

private slots:
    void accept();
    void help();
};

#endif // ENVWINDOW_H
