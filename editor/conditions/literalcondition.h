#ifndef LITERALCONDITION_H
#define LITERALCONDITION_H

#include "abstractcondition.h"

class LiteralCondition : public AbstractCondition
{
public:
    explicit LiteralCondition();
    LiteralCondition(const QString&);
    LiteralCondition(const QVariantMap&);
};

#endif // LITERALCONDITION_H
