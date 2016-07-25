#include "conditiontokenfactory.h"

#include "literalcondition.h"
#include "simplecondition.h"
#include "complexcondition.h"

ConditionToken* ConditionTokenFactory::createToken(const QVariant& data)
{
    QVariantMap _data = standardData(data);
    int type = _data.value("type").toInt();

    switch(type) {
        case ConditionTokenMetaType::Variable:
        case ConditionTokenMetaType::Value:
        case ConditionTokenMetaType::Number:
        case ConditionTokenMetaType::Boolean:
        case ConditionTokenMetaType::LogicalOperator:
            return new SimpleConditionToken(_data);
        case ConditionTokenMetaType::LiteralCondition:
        case ConditionTokenMetaType::SimpleCondition:
        case ConditionTokenMetaType::ComplexCondition:
            return createCondition(_data);
        default: break;
    }

    return 0;
}

AbstractCondition* ConditionTokenFactory::createCondition(const QVariant& data)
{
    QVariantMap _data = standardData(data, true);
    int type = _data.value("type").toInt();

    switch(type) {
        case ConditionTokenMetaType::Value:
        case ConditionTokenMetaType::LiteralCondition:
            return new LiteralCondition(_data);
        case  ConditionTokenMetaType::SimpleCondition:
            return new SimpleCondition(_data);
        case ConditionTokenMetaType::ComplexCondition:
            return new ComplexCondition(_data);
        default: break;
    }

    return 0;
}

QVariantMap ConditionTokenFactory::standardData(const QVariant & data, bool condition)
{
    bool ok = false;
    QVariantMap _data;

    if (data.type() == QVariant::String) {
        if (condition)
            _data.insert("type", ConditionTokenMetaType::LiteralCondition);
        else
            _data.insert("type", ConditionTokenMetaType::Value);
        _data.insert("value", data);
    }
    else if (data.type() == QVariant::Map) {
        _data = data.toMap();
        _data.value("type").toInt(&ok);
        if (!ok)
            _data.insert("type", ConditionTokenMetaType::Unknown);
    }

    return _data;
}
