#ifndef COMPLEXCONDITION_H
#define COMPLEXCONDITION_H

#include "abstractcondition.h"
#include "simplecondition.h"

class ComplexCondition : public AbstractCondition
{
public:
    explicit ComplexCondition();
    virtual ~ComplexCondition();
    ComplexCondition(AbstractCondition*);
    ComplexCondition(const QVariantMap&);

    void addCondition(ConditionLogicalOperator::Type, AbstractCondition*);
    QList<AbstractCondition*> conditions() const;
    AbstractCondition* conditionAt(int) const;
    ConditionToken* tokenAt(int) const;
    int indexOfToken(ConditionToken*) const;
    bool removeCondition(AbstractCondition*, bool del=false);
    void setCondition(AbstractCondition*);
    void clear(bool del=false);
    QList<ConditionToken*> tokens() const;
    bool isEmpty() const;
    virtual QString toString() const;
    virtual bool setValue(const QVariant &);
    virtual QVariant value() const;

private:
    void cleanup(int);

private:
    QList<ConditionToken*> mTokens;
    ComplexCondition(const ComplexCondition&);
    ComplexCondition& operator=(const ComplexCondition&);
};

#endif // COMPLEXCONDITION_H
