#include <QNetworkAccessManager>

class UpdateChecker : public QObject
{
    Q_OBJECT
public:
    UpdateChecker();
    ~UpdateChecker();
    void checkUpdate();
signals:
    void ready(const bool &isNewVersion, const QString &version);

private:
    QNetworkAccessManager *manager;
    void parseReply(QNetworkReply *reply);
    static bool isNewVersion(const QString &target, const QString &current);
};
