#include "complexcondition.h"

#include "conditiontokenfactory.h"

ComplexCondition::ComplexCondition() :
    AbstractCondition(ConditionTokenMetaType::ComplexCondition)
{
}

ComplexCondition::ComplexCondition(AbstractCondition* cond) :
    AbstractCondition(ConditionTokenMetaType::ComplexCondition)
{
    setCondition(cond);
}

ComplexCondition::ComplexCondition(const QVariantMap& data) :
    AbstractCondition(data)
{
    setValue(data.value("value"));
}

ComplexCondition::~ComplexCondition()
{
    clear(true);
}

void ComplexCondition::addCondition(ConditionLogicalOperator::Type op, AbstractCondition * cond)
{
    if (! cond)
        return;

    if (mTokens.size())
        mTokens.append(new ConditionToken(ConditionTokenMetaType::LogicalOperator, op));
    mTokens.append(cond);
}

QList<AbstractCondition*> ComplexCondition::conditions() const
{
    QList<AbstractCondition*> conditions;
    for(int i=0; i < mTokens.size(); i++) {
        ConditionToken* token = mTokens.at(i);
        if (token && (token->type() == ConditionTokenMetaType::SimpleCondition ||
                      token->type() == ConditionTokenMetaType::LiteralCondition ||
                      token->type() == ConditionTokenMetaType::ComplexCondition)) {
            conditions << dynamic_cast<AbstractCondition*>(token);
        }
    }

    return conditions;
}

AbstractCondition* ComplexCondition::conditionAt(int index) const
{
    QList<AbstractCondition*> conditions = this->conditions();
    if (index >= 0 && index < conditions.size())
        return conditions.at(index);
    return 0;
}

ConditionToken* ComplexCondition::tokenAt(int index) const
{
    if (index >= 0 && index < mTokens.size())
        return mTokens.at(index);
    return 0;
}

int ComplexCondition::indexOfToken(ConditionToken * token) const
{
    for(int i=0; i < mTokens.size(); i++) {
        if (mTokens.at(i) == token)
            return i;
    }

    return -1;
}

bool ComplexCondition::removeCondition(AbstractCondition * cond, bool del)
{
    ConditionToken* token = static_cast<AbstractCondition*>(cond);
    int index = mTokens.indexOf(token);
    if (index != -1) {
        mTokens.removeAt(index);
        if (token && del)
            delete token;
        cleanup(index);
        return true;
    }

    return false;
}

void ComplexCondition::clear(bool del)
{
    for(int i=mTokens.size()-1; i >= 0; --i) {
        ConditionToken* token = mTokens.takeAt(i);
        if (del)
            delete token;
    }

    mTokens.clear();
}

void ComplexCondition::setCondition(AbstractCondition * condition)
{
    clear(true);
    mTokens.append(condition);
}

bool ComplexCondition::setValue(const QVariant & value)
{
    if (value.type() != QVariant::List)
        return false;

    clear(true);

    QVariantList tokens = value.toList();
    ConditionToken* conditionToken = 0;
    foreach(const QVariant& token, tokens) {
        conditionToken = ConditionTokenFactory::createToken(token.toMap());
        if (conditionToken)
            mTokens.append(conditionToken);
    }

    return true;
}

QVariant ComplexCondition::value() const
{
    QVariantList value;
    foreach(ConditionToken* token, mTokens) {
        value << token->toMap();
    }
    return value;
}

QString ComplexCondition::toString() const
{
    QStringList textparts;
    foreach(ConditionToken* token, mTokens) {
        textparts << token->toString();
    }
    return textparts.join(" ");
}

QList<ConditionToken*> ComplexCondition::tokens() const
{
    return mTokens;
}

bool ComplexCondition::isEmpty() const
{
    return mTokens.isEmpty();
}

void ComplexCondition::cleanup(int index)
{
    ConditionToken* token = 0;

    if (index < mTokens.size() && mTokens.at(index)->type() == ConditionTokenMetaType::LogicalOperator) {
        token = mTokens.takeAt(index);
    }
    else if (mTokens.size() && mTokens.last()->type() == ConditionTokenMetaType::LogicalOperator) {
        token = mTokens.takeLast();
    }

    if (token)
        delete token;
}
