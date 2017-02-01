#include "actionmanagerbutton.h"

#include "actionmanagerdialog.h"

ActionManagerButton::ActionManagerButton(QWidget *parent) :
    QPushButton(parent)
{
    mActionManager = 0;
    mDefaultText = tr("None");
    setText(mDefaultText);
    setIcon(QIcon(":/media/object-edit.png"));
    setStyleSheet(QString("text-align: left; padding: 4px;"));
    connect(this, SIGNAL(clicked()), this, SLOT(onClicked()));
}

GameObjectManager* ActionManagerButton::actionManager() const
{
    return mActionManager;
}

void ActionManagerButton::setActionManager(GameObjectManager* actionManager)
{
    if (mActionManager == actionManager)
        return;

    if (mActionManager)
        mActionManager->disconnect(this);

    mActionManager = actionManager;
    updateText();

    if (mActionManager)
        connect(mActionManager, SIGNAL(destroyed()), this, SLOT(onActionManagerDestroyed()));
}

void ActionManagerButton::onClicked()
{
    if (!mActionManager)
        return;

    ActionManagerDialog dialog(mActionManager);
    ActionCatalogWidget* widget = dialog.actionCatalogWidget();
    if (widget)
        widget->hideActions(mActionFilter);
    dialog.exec();
    updateText();
    emit managingFinished();
}

void ActionManagerButton::updateText()
{
    QStringList parts;

    if (mActionManager) {
        foreach(GameObject* obj, mActionManager->objects()) {
            if (!obj)
                continue;

            const GameObjectMetaType* metaType = GameObjectMetaType::metaType(obj->type());
            if (metaType)
                parts.append(metaType->name());
        }
    }

    if (parts.isEmpty())
        setText(mDefaultText);
    else
        setText(parts.join(", "));
}

void ActionManagerButton::onActionManagerDestroyed()
{
    mActionManager = 0;
}

void ActionManagerButton::clear()
{
    setActionManager(0);
}

void ActionManagerButton::hideAction(GameObjectMetaType::Type type)
{
    if (!mActionFilter.contains(type)) {
        mActionFilter.append(type);
    }
}

void ActionManagerButton::hideActions(const QList<GameObjectMetaType::Type>& types)
{
    foreach(GameObjectMetaType::Type type, types) {
        hideAction(type);
    }
}

void ActionManagerButton::showAction(GameObjectMetaType::Type type)
{
    if (mActionFilter.contains(type)) {
        mActionFilter.removeAll(type);
    }
}

void ActionManagerButton::showActions(const QList<GameObjectMetaType::Type>& types)
{
    foreach(GameObjectMetaType::Type type, types) {
        showAction(type);
    }
}
