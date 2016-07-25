#ifndef CONDITIONTOKEN_METATYPE_H
#define CONDITIONTOKEN_METATYPE_H

class ConditionTokenMetaType
{
public:
    enum Type {
        Unknown=0,
        Variable,
        Value,
        String=Value,
        Number,
        Boolean,
        LogicalOperator=16,
        AbstractCondition,
        SimpleCondition,
        ComplexCondition,
        LiteralCondition
    };
};

#endif // CONDITIONTOKEN_METATYPE_H
