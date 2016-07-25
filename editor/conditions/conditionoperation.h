#ifndef CONDITIONOPERATION_H
#define CONDITIONOPERATION_H

#include <QString>
#include <QMap>

class ConditionOperation
{
public:
    enum Type {
        Unknown=0,
        Equal,
        NotEqual,
        Contains,
        GreaterThan,
        GreaterThanOrEqual,
        LesserThan,
        LesserThanOrEqual,
        IsTrue,
        IsFalse,
        IsDefined,
        IsUndefined
    };

    static QString toString(Type);
    static QMap<int, QString> operations();
};

#endif // CONDITIONOPERATION_H
