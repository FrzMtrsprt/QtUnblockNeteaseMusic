#ifndef PARAM_H
#define PARAM_H

#include <QVariant>

class Param : public QVariant
{
public:
      Param(){};
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

#endif // PARAM_H