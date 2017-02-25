#include "actionpool.h"

ActionPool::ActionPool(QObject *parent) :
    QObject(parent)
{
}

ActionPool::~ActionPool()
{
    emit destroyed(this);
}

QList<Action*> ActionPool::actions() const
{
    return mActions;
}

void ActionPool::addAction(Action * action)
{
    if (!action)
        return;

    connect(action, SIGNAL(destroyed(QObject*)), this, SLOT(onActionDestroyed(QObject*)));
    mActions.append(action);
}

bool ActionPool::removeAction(Action * action)
{
    int count = mActions.removeAll(action);

    if (count)
        return true;
    return false;
}

bool ActionPool::contains(Action * action) const
{
    return mActions.contains(action);
}

Action* ActionPool::actionFromParent(GameObject * parent) const
{
    if (!parent)
        return 0;

    foreach(Action* action, mActions) {
        if (action && action->parent() == parent)
            return action;
    }

    return 0;
}

int ActionPool::size() const
{
    return mActions.size();
}

void ActionPool::onActionDestroyed(QObject * object)
{
    Action* actionToRemove = 0;
    foreach(Action* action, mActions) {
        if (action == object) {
            actionToRemove = action;
            break;
        }
    }

    if (actionToRemove)
        mActions.removeAll(actionToRemove);

    if (mActions.isEmpty())
        deleteLater();
}
