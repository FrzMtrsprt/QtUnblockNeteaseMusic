#include "wizardpages.h"

#ifdef Q_OS_WIN
#include "utils/winutils.h"
#endif

#include <QButtonGroup>
#include <QCommandLinkButton>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>

using namespace Qt::Literals::StringLiterals;

WizardPage1::WizardPage1(QWidget *parent)
    : QWizardPage(parent)
{
    QCommandLinkButton *unmCommand = new QCommandLinkButton(this);
    unmCommand->setCheckable(true);
    unmCommand->setText(tr("UnblockNeteaseMusic CA certificate"));
    unmCommand->setDescription(tr("Install the certificate from UnblockNeteaseMusic."));
    QCommandLinkButton *otherCommand = new QCommandLinkButton(this);
    otherCommand->setCheckable(true);
    otherCommand->setText(tr("Other certificate"));
    otherCommand->setDescription(tr("Install the certificate from file."));

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(unmCommand);
    buttonGroup->addButton(otherCommand);
    buttonGroup->setId(unmCommand, 0);
    buttonGroup->setId(otherCommand, 1);

    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    vboxLayout->addWidget(unmCommand);
    vboxLayout->addWidget(otherCommand);

    setLayout(vboxLayout);
    setTitle(tr("Select certificate"));

    connect(buttonGroup, &QButtonGroup::buttonClicked,
            this, &WizardPage1::on_clicked);
}

WizardPage1::~WizardPage1()
{
}

int WizardPage1::nextId() const
{
    return findChild<QButtonGroup *>()->checkedId() == 0 ? 2 : 1;
}

bool WizardPage1::isComplete() const
{
    return findChild<QButtonGroup *>()->checkedId() != -1;
}

bool WizardPage1::validatePage()
{
    switch (findChild<QButtonGroup *>()->checkedId())
    {
    case 0:
        setField(u"fileName"_s, u":/certs/ca.crt"_s);
        break;
    case 1:
        setField(u"fileName"_s, QString());
        break;
    default:
        break;
    }
    return true;
}

void WizardPage1::on_clicked()
{
    if (findChild<QButtonGroup *>()->checkedId() == 0)
    {
        setButtonText(QWizard::NextButton, tr("&Install"));
    }
    else
    {
        setButtonText(QWizard::NextButton, wizard()->buttonText(QWizard::NextButton));
    }
    completeChanged();
}

WizardPage2::WizardPage2(QWidget *parent)
    : QWizardPage(parent)
{
    QLineEdit *lineEdit = new QLineEdit(this);
    lineEdit->setReadOnly(true);
    QPushButton *toolButton = new QPushButton(this);
    toolButton->setText(tr("Browse..."));

    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
    hboxLayout->addWidget(lineEdit);
    hboxLayout->addWidget(toolButton);

    setLayout(hboxLayout);
    setTitle(tr("Select certificate file"));
    setButtonText(QWizard::NextButton, tr("&Install"));

    connect(toolButton, &QPushButton::clicked,
            this, &WizardPage2::on_browse);
    connect(lineEdit, &QLineEdit::textChanged,
            this, &QWizardPage::completeChanged);

    registerField(u"fileName"_s, lineEdit);
}

WizardPage2::~WizardPage2()
{
}

int WizardPage2::nextId() const
{
    return 2;
}

bool WizardPage2::isComplete() const
{
    return !findChild<QLineEdit *>()->text().isEmpty();
}

void WizardPage2::on_browse()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Certificate files (*.crt)"));
    if (dialog.exec() == QDialog::Accepted)
    {
        findChild<QLineEdit *>()->setText(dialog.selectedFiles()[0]);
    }
}

WizardPage3::WizardPage3(QWidget *parent)
    : QWizardPage(parent)
{
    QPlainTextEdit *detailsEdit = new QPlainTextEdit(this);
    detailsEdit->setReadOnly(true);
    QLabel *errorLabel = new QLabel(this);
    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    vboxLayout->addWidget(errorLabel);
    vboxLayout->addWidget(detailsEdit);
}

WizardPage3::~WizardPage3()
{
}

void WizardPage3::initializePage()
{
    qDebug() << "Installing certificate " + field(u"fileName"_s).toString();
#ifdef Q_OS_WIN
    const auto [succeed, error, detail] = WinUtils::installCA(field(u"fileName"_s).toString());
#else
    const auto [succeed, error, detail] = std::make_tuple(false, u"Unsupported"_s, QString());
#endif
    setTitle(succeed ? tr("Success") : tr("Failed"));
    findChild<QLabel *>()->setText(error);
    findChild<QPlainTextEdit *>()->setPlainText(detail);
}
