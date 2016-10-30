/* Copyright (C) 2012-2014 Carlos Pais
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "branch.h"
#include "gameobjectfactory.h"
#include "literalcondition.h"
#include "conditiontokenfactory.h"

Branch::Branch(QObject *parent) :
    Action(parent)
{
    init();
}

Branch::Branch(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();
    loadInternal(data);
}

Branch::~Branch()
{
    if (mCondition)
        delete mCondition;
}

void Branch::init()
{
    setType(GameObjectMetaType::Branch);
    mCondition = new ComplexCondition();
}

void Branch::loadData(const QVariantMap & data, bool internal)
{
    if (!internal)
        Action::loadData(data, internal);

    QVariantList actions;

    if (data.contains("trueActions") && data.value("trueActions").type() == QVariant::List) {
        actions = data.value("trueActions").toList();
        foreach(const QVariant& actionData, actions) {
            if (actionData.type() != QVariant::Map)
                continue;
            Action* action = GameObjectFactory::createAction(actionData.toMap(), this);
            if (action)
                appendAction(action, true);
        }
    }

    if (data.contains("falseActions") && data.value("falseActions").type() == QVariant::List) {
        actions = data.value("falseActions").toList();
        foreach(const QVariant& actionData, actions) {
            if (actionData.type() != QVariant::Map)
                continue;
            Action* action = GameObjectFactory::createAction(actionData.toMap(), this);
            if (action)
                appendAction(action, false);
        }
    }

    if (data.contains("condition")) {
        setCondition(ConditionTokenFactory::createCondition(data.value("condition")));
    }
}

QVariantMap Branch::toJsonObject(bool internal) const
{
    QVariantMap data = Action::toJsonObject(internal);
    data.insert("condition", mCondition->toMap());

    QVariantList actions;
    if (! mTrueActions.isEmpty()){
        foreach(Action* action, mTrueActions)
            actions.append(action->toJsonObject(internal));
    }
    data.insert("trueActions", actions);
    actions.clear();

    if (! mFalseActions.isEmpty()){
        foreach(Action* action, mFalseActions)
            actions.append(action->toJsonObject(internal));
    }
    data.insert("falseActions", actions);
    actions.clear();

    return data;
}

AbstractCondition* Branch::condition() const
{
    return mCondition;
}

void Branch::setCondition(AbstractCondition *condition)
{
    if (mCondition == condition)
        return;

    if (mCondition)
        delete mCondition;
    mCondition = condition;
    updateDisplayText();
}

void Branch::setCondition(const QString& condition)
{
    setCondition(new LiteralCondition(condition));
}

QList<Action*> Branch::actions(bool cond) const
{
    if (cond)
        return mTrueActions;
    return mFalseActions;
}

void Branch::appendAction(Action* action, bool cond)
{
    if (cond)
        mTrueActions.append(action);
    else
        mFalseActions.append(action);
    updateDisplayText();
}

Action* Branch::action(int index, bool cond) const
{
    if (cond) {
        if (index < mTrueActions.size())
            return mTrueActions[index];
    }
    else if (index < mFalseActions.size())
        return mFalseActions[index];

    return 0;
}

void Branch::removeAction(int index, bool cond, bool del){
    Action* action = 0;

    if (cond) {
        if (index < mTrueActions.size())
            action = mTrueActions.takeAt(index);
    }
    else if (index < mFalseActions.size())
        action = mFalseActions.takeAt(index);

    if (action && del)
        action->deleteLater();
}

void Branch::updateDisplayText()
{
    QString condition("");
    QString trueAction(tr("Do Nothing"));
    QString falseAction(tr("Do Nothing"));
    QStringList trueActions;
    QStringList falseActions;
    const GameObjectMetaType* metatype = 0;

    if (mCondition)
        condition = mCondition->toString();

    for(int i=0; i < mTrueActions.size(); i++) {
        metatype = GameObjectMetaType::metaType(mTrueActions.at(i)->type());
        if (metatype)
            trueActions.append(metatype->name());
    }

    if (!trueActions.isEmpty())
        trueAction = trueActions.join(", ");

    for(int i=0; i < mFalseActions.size(); i++) {
        metatype = GameObjectMetaType::metaType(mFalseActions.at(i)->type());
        if (metatype)
            falseActions.append(metatype->name());
    }

    if (!falseActions.isEmpty())
        falseAction = falseActions.join(", ");

    QString text = QString(tr("If %1\nThen: %2\nElse: %3")).arg(condition).arg(trueAction).arg(falseAction);
    setDisplayText(text);
    emit dataChanged();
}
