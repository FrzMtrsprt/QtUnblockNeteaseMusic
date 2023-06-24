#ifndef CAWIZARDPAGES_H
#define CAWIZARDPAGES_H

#include <QWizard>

class WizardPage1 : public QWizardPage
{
    Q_OBJECT
public:
    WizardPage1(QWidget *parent = nullptr);
    ~WizardPage1();

private:
    int nextId() const;
    bool isComplete() const;
    bool validatePage();
};

class WizardPage2 : public QWizardPage
{
    Q_OBJECT
public:
    WizardPage2(QWidget *parent = nullptr);
    ~WizardPage2();

private:
    int nextId() const;
    bool isComplete() const;
private slots:
    void on_browse();
};

class WizardPage3 : public QWizardPage
{
    Q_OBJECT
public:
    WizardPage3(QWidget *parent = nullptr);
    ~WizardPage3();

private:
    void initializePage();
};

#endif // CAWIZARDPAGES_H
