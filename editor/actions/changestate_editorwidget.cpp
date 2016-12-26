#include "changestate_editorwidget.h"

#include "character.h"

#include <QDebug>

ChangeStateEditorWidget::ChangeStateEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mCharacterBox = new ObjectComboBox(this);
    mCharacterBox->addTypeFilter(GameObjectMetaType::Character);
    mStateBox = new QComboBox(this);

    beginGroup(tr("Change State Editor"));
    appendRow(tr("Character"), mCharacterBox);
    appendRow(tr("State"), mStateBox);
    endGroup();
    resizeColumnToContents(0);

    connect(mCharacterBox, SIGNAL(objectChanged(Object*)), this, SLOT(onObjectChosen(Object*)));
    connect(mStateBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onStateChosen(const QString&)));
}

void ChangeStateEditorWidget::updateData(GameObject* action)
{
    ActionEditorWidget::updateData(action);
    ChangeState* changeStateAction = qobject_cast<ChangeState*>(action);
    if (! changeStateAction)
        return;

    mCharacterBox->loadFromAction(changeStateAction);
    mStateBox->clear();
    Object* currObject = changeStateAction->sceneObject();
    if (currObject) {
        Character* character = qobject_cast<Character*>(currObject);
        if (character)
            mStateBox->addItems(character->states());
    }

    if (! changeStateAction->state().isEmpty())
        mStateBox->setCurrentIndex(mStateBox->findText(changeStateAction->state()));
}

void ChangeStateEditorWidget::onObjectChosen(Object* obj)
{
    ChangeState* action = qobject_cast<ChangeState*>(mGameObject);
    if (! action)
        return;

    mStateBox->clear();
    action->setSceneObject(obj);
    action->setState("");
    Character* character = qobject_cast<Character*>(action->sceneObject());
    if (character)
        mStateBox->addItems(character->states());
}

void ChangeStateEditorWidget::onStateChosen(const QString& state)
{
    ChangeState* action = qobject_cast<ChangeState*>(mGameObject);
    if (action)
        action->setState(state);
}
