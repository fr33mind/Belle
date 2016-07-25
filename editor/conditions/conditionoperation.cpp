#include "conditionoperation.h"

QString ConditionOperation::toString(ConditionOperation::Type type)
{
    switch(type)
    {
        case Equal: return "equal to";
        case NotEqual: return "not equal to";
        case Contains: return "contains";
        case GreaterThan: return "greater than";
        case GreaterThanOrEqual: return "greater than or equal to";
        case LesserThan: return "lesser than";
        case LesserThanOrEqual: return "lesser than or equal to";
        case IsTrue: return "is true";
        case IsFalse: return "is false";
        case IsDefined: return "is defined";
        case IsUndefined: return "is undefined";
        default: return "";
    }

    return "";
}

QMap<int, QString> ConditionOperation::operations()
{
    QMap<int, QString> operations;
    for(int i=1; i <= (int) IsUndefined; i++) {
        operations.insert(i, toString((Type) i));
    }
    return operations;
}
