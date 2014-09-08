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
    mChangeStateAction = 0;
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

    mCharacterBox->setCurrentIndex(-1);
    if (currIndex != -1) {
        mCharacterBox->setCurrentIndex(currIndex);
        if (! changeStateAction->state().isEmpty())
            mStateBox->setCurrentIndex(mStateBox->findText(changeStateAction->state()));
    }

    mChangeStateAction = changeStateAction;

    if (currIndex == -1 && mCharacterBox->count())
        mCharacterBox->setCurrentIndex(0);
}

void ChangeStateEditorWidget::onObjectChosen(const QString& name)
{
    if (! mChangeStateAction)
        return;

    mStateBox->clear();
    mChangeStateAction->setSceneObject(name);
    Character* character = qobject_cast<Character*>(mChangeStateAction->sceneObject());
    if (character)
        mStateBox->addItems(character->states());
}

void ChangeStateEditorWidget::onStateChosen(const QString& state)
{
    if (! mChangeStateAction)
        return;
    mChangeStateAction->setState(state);
}
