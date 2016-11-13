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

    //internal data passed between resources and clones
    if (data.contains("_appendAction") && data.value("_appendAction").type() == QVariant::Map) {
        QVariantMap actionMap = data.value("_appendAction").toMap();
        QVariant actionData = actionMap.value("actionData");
        QVariant condition = actionMap.value("condition");
        Action* newAction = 0;
        Action* action = actionMap.value("action").value<Action*>();

        if (action && actionData.type() == QVariant::Map && condition.type() == QVariant::Bool) {
            newAction = GameObjectFactory::createAction(actionData.toMap(), this);
            if(action->isResource())
                newAction->setResource(action);
            else if (isResource())
                action->setResource(newAction);

            appendAction(newAction, condition.toBool());
        }
    }

    if (data.contains("_removeAction") && data.value("_removeAction").type() == QVariant::Map) {
        QVariantMap actionMap = data.value("_removeAction").toMap();
        QVariant condition = actionMap.value("condition");
        bool del = actionMap.value("delete", false).toBool();
        Action* action = actionMap.value("action").value<Action*>();

        if (action && condition.type() == QVariant::Bool) {
            bool cond = condition.toBool();
            if (isResource()) {
                removeAction(qobject_cast<Action*>(action->resource()), cond, del);
            }
            else {
                QList<Action*> _actions = this->actions(cond);
                foreach(Action* _action, _actions) {
                    if (_action && _action->resource() == action)
                        removeAction(_action, cond, del);
                }
            }
        }
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

void Branch::connectToResource()
{
    Action::connectToResource();
    Branch * resource = qobject_cast<Branch*>(this->resource());
    if (resource) {
        connectActions(resource, true);
        connectActions(resource, false);
    }
}

void Branch::connectActions(Branch* resource, bool cond)
{
    if (!resource)
        return;

    QList<Action*> resActions, currActions;
    resActions = resource->actions(cond);
    currActions = actions(cond);

    for(int i=0; i < resActions.size() && i < currActions.size(); i++) {
        if (!currActions.at(i) || !resActions.at(i))
            continue;

        if (currActions.at(i)->type() == resActions.at(i)->type()) {
            currActions.at(i)->setResource(resActions.at(i));
            connect(this, SIGNAL(syncChanged(bool)), currActions.at(i), SLOT(setSync(bool)));
        }
    }
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

    QVariantMap data;
    if (mCondition)
        data = mCondition->toMap();
    notify("condition", data);
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
    if (!action)
        return;

    if (cond)
        mTrueActions.append(action);
    else
        mFalseActions.append(action);

    connect(this, SIGNAL(syncChanged(bool)), action, SLOT(setSync(bool)));
    updateDisplayText();

    QVariantMap data;
    data.insert("action", QVariant::fromValue(qobject_cast<QObject*>(action)));
    data.insert("actionData", action->toJsonObject());
    data.insert("condition", cond);
    notify("_appendAction", data);
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
    if (loadBlocked())
        return;

    Action* action = 0;

    if (cond) {
        if (index < mTrueActions.size())
            action = mTrueActions.takeAt(index);
    }
    else if (index < mFalseActions.size())
        action = mFalseActions.takeAt(index);

    if (action) {
        action->disconnect(this);
        disconnect(action);

        QVariantMap data;
        data.insert("action", QVariant::fromValue(qobject_cast<QObject*>(action)));
        data.insert("condition", cond);
        data.insert("delete", del);
        notify("_removeAction", data);

        if (del)
            delete action;
    }
}

void Branch::removeAction(Action* action, bool cond, bool del)
{
    int index = -1;

    if (cond)
        index = mTrueActions.indexOf(action);
    else
        index = mFalseActions.indexOf(action);

    removeAction(index, cond, del);
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
}

void Branch::onConditionChanged()
{
    updateDisplayText();

    if (mCondition)
        notify("condition", mCondition->toMap());
}
