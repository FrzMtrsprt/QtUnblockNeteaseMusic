#include "cawizard.h"
#include "cawizardpages.h"

#include <QApplication>
#include <QResizeEvent>

CaWizard::CaWizard(QWidget *parent)
    : QWizard(parent)
{
    addPage(new WizardPage1(this));
    addPage(new WizardPage2(this));
    addPage(new WizardPage3(this));
    setWizardStyle(QWizard::ModernStyle);
}

CaWizard::~CaWizard()
{
}

void CaWizard::showEvent(QShowEvent *event)
{
    QWizard::showEvent(event);
    // Reference: https://bugreports.qt.io/browse/QTBUG-10478
    QApplication::postEvent(this, new QResizeEvent(size(), size()));
}
