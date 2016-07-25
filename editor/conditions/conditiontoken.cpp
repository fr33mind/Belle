#include "conditiontoken.h"

ConditionToken::ConditionToken(ConditionTokenMetaType::Type type, const QVariant& value)
{
    mType = type;
    mValue = value;
}

ConditionToken::ConditionToken(const QVariantMap& data)
{
    bool ok = false;
    int type = data.value("type").toInt(&ok);
    mType = ok ? (ConditionTokenMetaType::Type) type : ConditionTokenMetaType::Unknown;
    setValue(data.value("value"));
}

ConditionToken::ConditionToken()
{
    mType = ConditionTokenMetaType::Unknown;
}

ConditionTokenMetaType::Type ConditionToken::type() const
{
    return mType;
}

bool ConditionToken::setValue(const QVariant & value)
{
    switch(value.type()) {
        case QVariant::String:
        case QVariant::Int:
        case QVariant::Double:
        case QVariant::Bool:
            mValue = value;
            return true;
        default: break;
    }

    return false;
}

QVariant ConditionToken::value() const
{
    return mValue;
}

bool ConditionToken::isEmpty() const
{
    return toString().isEmpty();
}

QString ConditionToken::toString() const
{
    switch(mType) {
        case ConditionTokenMetaType::LogicalOperator:
            return ConditionLogicalOperator::toString(mValue.toInt());
        case ConditionTokenMetaType::Variable:
            return "$" + mValue.toString();
        case ConditionTokenMetaType::Value:
            return QString("\"%1\"").arg(mValue.toString());
        default:
            return mValue.toString();
    }
}

QVariantMap ConditionToken::toMap() const
{
    QVariantMap data;
    data.insert("type", (int) mType);
    data.insert("value", value());
    return data;
}

SimpleConditionToken::SimpleConditionToken(ConditionTokenMetaType::Type type, const QVariant& value) :
    ConditionToken(type, value)
{
}

SimpleConditionToken::SimpleConditionToken() :
    ConditionToken()
{
}

SimpleConditionToken::SimpleConditionToken(const QVariantMap& data) :
    ConditionToken(data)
{
}
