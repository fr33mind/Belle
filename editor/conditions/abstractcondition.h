#ifndef ABSTRACTCONDITION_H
#define ABSTRACTCONDITION_H

#include "conditiontoken.h"

class AbstractCondition : public ConditionToken
{
public:
    bool isComplex() const;
    bool isLiteral() const;
    bool isSimple() const;

protected:
    AbstractCondition(ConditionTokenMetaType::Type);
    AbstractCondition(const QVariantMap&);

};

#endif // ABSTRACTCONDITION_H
