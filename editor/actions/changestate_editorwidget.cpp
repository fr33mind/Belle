#include "changestate_editorwidget.h"

#include "character.h"

#include <QDebug>

ChangeStateEditorWidget::ChangeStateEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mCharacterBox = new QComboBox(this);
    mStateBox = new QComboBox(this);

    beginGroup(tr("Change State Editor"));
    appendRow(tr("Character"), mCharacterBox);
    appendRow(tr("State"), mStateBox);
    endGroup();
    resizeColumnToContents(0);

    connect(mCharacterBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onObjectChosen(const QString&)));
    connect(mStateBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onStateChosen(const QString&)));
}

void ChangeStateEditorWidget::updateData(Action * action)
{
    ActionEditorWidget::updateData(action);
    mAction = 0;
    ChangeState* changeStateAction = qobject_cast<ChangeState*>(action);
    if (! changeStateAction)
        return;

    QList<Object*> objects = changeStateAction->availableObjects();
    Object* currObject = changeStateAction->sceneObject();
    int currIndex = -1;
    mCharacterBox->clear();
    foreach(Object* object, objects) {
        if (currObject && currObject == object)
            currIndex = mCharacterBox->count();
        mCharacterBox->addItem(object->name());
    }

    if (currIndex != -1)
        mCharacterBox->setCurrentIndex(currIndex);
    else
        mCharacterBox->setCurrentIndex(-1);

    mStateBox->clear();
    if (currObject) {
        Character* character = qobject_cast<Character*>(currObject);
        if (character)
            mStateBox->addItems(character->states());
    }

    if (! changeStateAction->state().isEmpty())
        mStateBox->setCurrentIndex(mStateBox->findText(changeStateAction->state()));

    mAction = action;

    if (! currObject && mCharacterBox->count())
        mCharacterBox->setCurrentIndex(0);
}

void ChangeStateEditorWidget::onObjectChosen(const QString& name)
{
    ChangeState* action = qobject_cast<ChangeState*>(mAction);
    if (! action)
        return;

    mStateBox->clear();
    action->setSceneObject(name);
    action->setState("");
    Character* character = qobject_cast<Character*>(action->sceneObject());
    if (character)
        mStateBox->addItems(character->states());
}

void ChangeStateEditorWidget::onStateChosen(const QString& state)
{
    ChangeState* action = qobject_cast<ChangeState*>(mAction);
    if (action)
        action->setState(state);
}
