#ifndef SIMPLECONDITION_H
#define SIMPLECONDITION_H

#include "abstractcondition.h"
#include "conditionoperation.h"

class SimpleCondition : public AbstractCondition
{
public:
    SimpleCondition(const SimpleConditionToken&, ConditionOperation::Type, const SimpleConditionToken& right=SimpleConditionToken());
    SimpleCondition(const QVariantMap&);

    ConditionToken leftOperand() const;
    void setLeftOperand(const ConditionToken&);
    ConditionOperation::Type operation() const;
    void setOperation(ConditionOperation::Type);
    ConditionToken rightOperand() const;
    void setRightOperand(const ConditionToken&);
    virtual QString toString() const;
    virtual bool setValue(const QVariant &);
    virtual QVariant value() const;

private:
    ConditionToken mLeftOperand;
    ConditionOperation::Type mOperation;
    ConditionToken mRightOperand;
};

#endif // SIMPLECONDITION_H
