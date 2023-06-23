#ifndef CAWIZARD_H
#define CAWIZARD_H

#include <QWizard>

class CaWizard : public QWizard
{
    Q_OBJECT

public:
    explicit CaWizard(QWidget *parent = nullptr);
    ~CaWizard();
private:
    void showEvent(QShowEvent *event);

};


#endif // CAWIZARD_H
