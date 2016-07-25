#include "simplecondition.h"

SimpleCondition::SimpleCondition(const SimpleConditionToken& left, ConditionOperation::Type op, const SimpleConditionToken& right) :
    AbstractCondition(ConditionTokenMetaType::SimpleCondition)
{
    mLeftOperand = left;
    mOperation = op;
    mRightOperand = right;
}

SimpleCondition::SimpleCondition(const QVariantMap& data) :
    AbstractCondition(data)
{
    setValue(data.value("value"));
}

ConditionToken SimpleCondition::leftOperand() const
{
    return mLeftOperand;
}

void SimpleCondition::setLeftOperand(const ConditionToken& left)
{
    mLeftOperand = left;
}

ConditionOperation::Type SimpleCondition::operation() const
{
    return mOperation;
}

void SimpleCondition::setOperation(ConditionOperation::Type op)
{
    mOperation = op;
}

ConditionToken SimpleCondition::rightOperand() const
{
    return mRightOperand;
}

void SimpleCondition::setRightOperand(const ConditionToken& right)
{
    mRightOperand = right;
}

bool SimpleCondition::setValue(const QVariant & value)
{
    if (value.type() != QVariant::Map)
        return false;

    QVariantMap data = value.toMap();
    mLeftOperand = SimpleConditionToken(data.value("leftOperand").toMap());
    mOperation = (ConditionOperation::Type) data.value("operation", ConditionOperation::Unknown).toInt();
    if (data.contains("rightOperand"))
        mRightOperand = SimpleConditionToken(data.value("rightOperand").toMap());

    return true;
}

QVariant SimpleCondition::value() const
{
    QVariantMap data;
    data.insert("leftOperand", mLeftOperand.toMap());
    data.insert("operation", mOperation);
    if (!mRightOperand.isEmpty())
        data.insert("rightOperand", mRightOperand.toMap());
    return data;
}

QString SimpleCondition::toString() const
{
    QStringList textparts;
    textparts << mLeftOperand.toString();
    textparts << ConditionOperation::toString(mOperation);
    textparts << mRightOperand.toString();
    return textparts.join(" ").trimmed();
}
