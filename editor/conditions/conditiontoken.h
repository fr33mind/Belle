#ifndef CONDITIONTOKEN_H
#define CONDITIONTOKEN_H

#include "conditiontokenmetatype.h"
#include "conditionlogicaloperator.h"

#include <QVariant>

class ConditionToken
{
public:
    ConditionToken(ConditionTokenMetaType::Type, const QVariant& value=QVariant());
    ConditionToken(const QVariantMap&);
    explicit ConditionToken();
    virtual ~ConditionToken() {}

    ConditionTokenMetaType::Type type() const;
    bool isEmpty() const;
    virtual bool setValue(const QVariant&);
    virtual QVariant value() const;
    virtual QString toString() const;
    QVariantMap toMap() const;

private:
    ConditionTokenMetaType::Type mType;
    QVariant mValue;
};

class SimpleConditionToken : public ConditionToken
{
public:
    SimpleConditionToken(ConditionTokenMetaType::Type, const QVariant& value=QVariant());
    SimpleConditionToken(const QVariantMap&);
    explicit SimpleConditionToken();
};

#endif // CONDITIONTOKEN_H
