#ifndef CONDITIONLOGICALOPERATOR_H
#define CONDITIONLOGICALOPERATOR_H

#include <QString>

class ConditionLogicalOperator
{
public:
    enum Type {
        And=0,
        Or
    };

    static QString toString(Type);
    static QString toString(int);
};

#endif // CONDITIONLOGICALOPERATOR_H
