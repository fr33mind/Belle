#include "abstractcondition.h"

AbstractCondition::AbstractCondition(ConditionTokenMetaType::Type type) :
    ConditionToken(type)
{
}

AbstractCondition::AbstractCondition(const QVariantMap& data) :
    ConditionToken(data)
{
}

bool AbstractCondition::isComplex() const
{
    return type() == ConditionTokenMetaType::ComplexCondition;
}

bool AbstractCondition::isLiteral() const
{
    return type() == ConditionTokenMetaType::LiteralCondition;
}

bool AbstractCondition::isSimple() const
{
    return type() == ConditionTokenMetaType::SimpleCondition;
}
