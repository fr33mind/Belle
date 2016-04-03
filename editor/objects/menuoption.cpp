#include "menuoption.h"

#include "action_info_manager.h"

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

    if (data.contains("actions") && data.value("actions").type() == QVariant::List) {
        QVariantList actions = data.value("actions").toList();
        for(int i=0; i < actions.size(); i++)
            addAction(ActionInfoManager::typeToAction(actions.at(i).toMap(), this));
    }

    if (data.contains("condition") && data.value("condition").type() == QVariant::String) {
        setCondition(data.value("condition").toString());
    }
}

void MenuOption::init()
{
    setType(GameObjectMetaType::MenuOption);
}

QList<Action *> MenuOption::actions() const
{
    return mActions;
}

void MenuOption::setActions(const QList<Action *> & actions)
{
    mActions = actions;
}

void MenuOption::addAction(Action* action)
{
    mActions.append(action);
}

void MenuOption::removeActionAt(int index)
{
    if (index >= 0 && index < mActions.size()) {
        Action* action = mActions.takeAt(index);
        if (action)
            action->deleteLater();
    }
}

QString MenuOption::condition() const
{
    return mCondition;
}

void MenuOption::setCondition(const QString& condition)
{
    mCondition = condition;
}

QVariantMap MenuOption::toJsonObject(bool internal)
{
    QVariantMap option = Button::toJsonObject(internal);
    QVariantList actions;

    for(int i=0; i < mActions.size(); i++) {
        actions.append(mActions[i]->toJsonObject(internal));
    }

    option.insert("actions", actions);
    if (! mCondition.isEmpty())
        option.insert("condition", mCondition);
    return option;
}
