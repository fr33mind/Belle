#include "conditionlogicaloperator.h"

QString ConditionLogicalOperator::toString(ConditionLogicalOperator::Type type)
{
    switch(type) {
        case ConditionLogicalOperator::And: return "and";
        case ConditionLogicalOperator::Or: return "or";
        default: return "";
    }
}

QString ConditionLogicalOperator::toString(int type)
{
    return toString((ConditionLogicalOperator::Type) type);
}
