#include "menuoption.h"

#include "gameobjectfactory.h"

MenuOption::MenuOption(const QString& text, QObject* parent) :
    Button(text, parent)
{
    init();
}

MenuOption::MenuOption(const QString& text, const QList<Action*>& actions, const QString& condition, QObject* parent) :
    Button(text, parent)
{
    init();
    setActions(actions);
    setCondition(condition);
}

MenuOption::MenuOption(const QVariantMap& data, QObject* parent) :
    Button(data, parent)
{
    init();
    loadInternal(data);
}

void MenuOption::init()
{
    setType(GameObjectMetaType::MenuOption);
    mActionManager = createActionManager();
}

GameObjectManager* MenuOption::createActionManager()
{
    GameObjectManager* actionManager = new GameObjectManager(this);
    actionManager->setUniqueNames(false);
    actionManager->setAllowEmptyNames(true);
    actionManager->setObjectsParent(this);
    connect(actionManager, SIGNAL(destroyed()), this, SLOT(onActionManagerDestroyed()));
    return actionManager;
}

void MenuOption::loadData(const QVariantMap & data, bool internal)
{
    if (!internal)
        Button::loadData(data, internal);

    if (data.contains("actions") && data.value("actions").type() == QVariant::List) {
        QVariantList actions = data.value("actions").toList();
        for(int i=0; i < actions.size(); i++)
            addAction(GameObjectFactory::createAction(actions.at(i).toMap(), this));
    }

    if (data.contains("condition") && data.value("condition").type() == QVariant::String) {
        setCondition(data.value("condition").toString());
    }
}

QList<Action *> MenuOption::actions() const
{
    QList<Action*> actions;
    Action* action = 0;

    if (!mActionManager)
        return actions;

    foreach(GameObject* obj, mActionManager->objects()) {
        action = qobject_cast<Action*>(obj);
        if (action)
            actions.append(action);
    }

    return actions;
}

void MenuOption::setActions(const QList<Action *> & actions)
{
    if (!mActionManager)
        return;

    QList<GameObject*> prevObjects = mActionManager->objects();
    mActionManager->clear();

    foreach(Action* action, actions) {
        mActionManager->add(action);
        if (prevObjects.contains(action)) {
            prevObjects.removeAll(action);
        }
    }

    foreach(GameObject* obj, prevObjects) {
        if (obj)
            delete obj;
    }
}

GameObjectManager* MenuOption::actionManager() const
{
    return mActionManager;
}

void MenuOption::addAction(Action* action)
{
    if (!mActionManager)
        return;

    mActionManager->add(action);
    emit dataChanged();
}

void MenuOption::removeActionAt(int index)
{
    if (!mActionManager)
        return;

    mActionManager->removeAt(index, true);
    emit dataChanged();
}

QString MenuOption::condition() const
{
    return mCondition;
}

void MenuOption::setCondition(const QString& condition)
{
    if (mCondition != condition) {
        mCondition = condition;
        emit dataChanged();
    }
}

QVariantMap MenuOption::toJsonObject(bool internal) const
{
    QVariantMap option = Button::toJsonObject(internal);
    QVariantList actions;

    if (mActionManager) {
        for(int i=0; i < mActionManager->size(); i++) {
            actions.append(mActionManager->objectAt(i)->toJsonObject(internal));
        }
    }

    option.insert("actions", actions);
    if (! mCondition.isEmpty())
        option.insert("condition", mCondition);
    return option;
}

void MenuOption::onActionManagerDestroyed()
{
    mActionManager = 0;
}
