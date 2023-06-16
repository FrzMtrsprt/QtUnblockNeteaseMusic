#ifndef PARAM_H
#define PARAM_H

#include <QVariant>

using namespace Qt::Literals::StringLiterals;

class Param : public QVariant
{
public:
      Param() : QVariant(){};
      Param(const QString &prefix,
            const QMetaType::Type &typeId,
            const QVariant &value = QVariant())
          : QVariant(value)
      {
            this->prefix = prefix;
            this->typeId = typeId;
      }
      ~Param(){};

      QString prefix;
      QMetaType::Type typeId;
};

class Params : public QHash<const char *, Param>
{
public:
      Params() : QHash<const char *, Param>(){};
      ~Params(){};

      Param &operator[](const char *key)
      {
            // Check if key is valid for type safety
            if (!keys.contains(key))
            {
                  qWarning("Invalid key: %s", key);
            }
            return QHash<const char *, Param>::operator[](key);
      };

      const QList<const char *> keys{
          "port", "address", "url", "host", "sources",
          "strict", "token", "endpoint", "cnrelay"};
};

#endif // PARAM_H
