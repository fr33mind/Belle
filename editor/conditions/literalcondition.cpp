#include "literalcondition.h"

LiteralCondition::LiteralCondition() :
    AbstractCondition(ConditionTokenMetaType::LiteralCondition)
{
}

LiteralCondition::LiteralCondition(const QString& condition) :
    AbstractCondition(ConditionTokenMetaType::LiteralCondition)
{
    setValue(condition);
}

LiteralCondition::LiteralCondition(const QVariantMap& data) :
    AbstractCondition(data)
{
    setValue(data.value("value"));
}
