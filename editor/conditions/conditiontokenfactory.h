#ifndef CONDITIONTOKENFACTORY_H
#define CONDITIONTOKENFACTORY_H

#include "conditiontoken.h"
#include "abstractcondition.h"

class ConditionToken;
class AbstractCondition;

class ConditionTokenFactory
{
public:
    static ConditionToken* createToken(const QVariant&);
    static AbstractCondition* createCondition(const QVariant&);
    static QVariantMap standardData(const QVariant&, bool condition=false);
};

#endif // CONDITIONTOKENFACTORY_H
