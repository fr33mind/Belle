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

#include "dialogue_editor_widget.h"

#include "character.h"
#include "dialoguebox.h"

#include <QDebug>

DialogueEditorWidget::DialogueEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mChooseCharacterWidget = new QComboBox(this);
    mChooseTextBoxWidget = new QComboBox(this);
    mChooseCharacterWidget->setEditable(true);
    mTextEdit = new QTextEdit(this);
    mWaitCheckBox = new QCheckBox(this);

    beginGroup("Dialogue Action");
    appendRow(tr("Character"), mChooseCharacterWidget);
    appendRow(tr("Text Box/Dialogue Box"), mChooseTextBoxWidget);
    appendRow(tr("Phrase"), mTextEdit);
    appendRow(tr("Wait on Finished"), mWaitCheckBox);
    endGroup();

    mTextEdit->setMaximumHeight(mTextEdit->height()/2);

    connect(mTextEdit, SIGNAL(textChanged()), this, SLOT(onTextEdited()));
    connect(mChooseTextBoxWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(onTextBoxChanged(int)));
    connect(mChooseTextBoxWidget, SIGNAL(highlighted(int)), this, SLOT(onTextBoxHighlighted(int)));
    connect(mChooseCharacterWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(onCharacterChanged(int)));
    connect(mChooseCharacterWidget, SIGNAL(highlighted(int)), this, SLOT(onCharacterHighlighted(int)));
    connect(mChooseCharacterWidget, SIGNAL(editTextChanged(const QString&)), this, SLOT(onCharacterNameChanged(const QString&)));
    connect(mWaitCheckBox, SIGNAL(clicked(bool)), this, SLOT(onWaitOnFinishedChanged(bool)));

    if (mChooseTextBoxWidget->view())
        mChooseTextBoxWidget->view()->installEventFilter(this);
    if (mChooseCharacterWidget->view())
        mChooseCharacterWidget->view()->installEventFilter(this);

    this->resizeColumnToContents(0);
}


void DialogueEditorWidget::updateData(Action * action)
{   
    Dialogue* dialogue = qobject_cast<Dialogue*> (action);
    if (! dialogue)
        return;

    ActionEditorWidget::updateData(action);
    mAction = 0;
    Character* character = 0;
    Object* object = 0;

    mChooseCharacterWidget->clear();
    mChooseCharacterWidget->clearEditText();
    mChooseTextBoxWidget->clear();
    mOutputBoxes.clear();
    mCharacters.clear();

    if (! dialogue->characterName().isEmpty()) {
        if (dialogue->character()) {
            mChooseCharacterWidget->addItem(dialogue->characterName());
            mCharacters.append(dialogue->character());
        }
        else {
            mChooseCharacterWidget->setEditText(dialogue->characterName());
        }
    }

    if (dialogue->sceneObject()) {
        object = dialogue->sceneObject();
        if (isValidOutputBox(object)) {
            mChooseTextBoxWidget->addItem(object->objectName());
            mOutputBoxes.append(object);
        }
    }

    Scene * scene = dialogue->scene();
    if (! scene)
        return;
    QList<Object*> objects = scene->objects();
    for (int i=0; i < objects.size(); i++) {
        character = qobject_cast<Character*>(objects[i]);
        if (character && character != dialogue->character()) {
            mChooseCharacterWidget->addItem(character->objectName());
            mCharacters.append(character);
            continue;
        }

        if (dialogue->sceneObject() != objects[i] && isValidOutputBox(objects[i])){
            mChooseTextBoxWidget->addItem(objects[i]->objectName());
            mOutputBoxes.append(objects[i]);
        }
    }

    if (! mChooseTextBoxWidget->count())
        mTextEdit->setEnabled(false);
    else {
        mTextEdit->setEnabled(true);
        dialogue->setSceneObject(mOutputBoxes[mChooseTextBoxWidget->currentIndex()]);
        mTextEdit->setText(dialogue->text());
    }

    if (dialogue->character())
        mChooseCharacterWidget->setCurrentIndex(0);
    else
        mChooseCharacterWidget->setEditText(dialogue->characterName());

    mWaitCheckBox->setChecked(dialogue->mouseClickOnFinish());

    //only set currentAction after updating all the widgets
    //otherwise updating the widgets would mess up the currentAction's data.
    mAction = action;
    setTextInOutputBox();
}

void DialogueEditorWidget::onTextEdited()
{
    Dialogue* dialogue = qobject_cast<Dialogue*> (mAction);
    if (! mTextEdit || ! dialogue)
        return;

    dialogue->setText(mTextEdit->toPlainText());
}

void DialogueEditorWidget::onTextBoxChanged(int index)
{
    if (mAction && index >= 0 && index < mOutputBoxes.size()) {
        mAction->setSceneObject(mOutputBoxes[index]);
        //onTextEdited();
    }
}

void DialogueEditorWidget::onCharacterChanged(int index)
{
    Dialogue* dialogue = qobject_cast<Dialogue*> (mAction);
    if (dialogue && index >= 0 && index < mCharacters.size() && mCharacters[index]) {
        dialogue->setCharacter(mCharacters[index]);
    }
}

void DialogueEditorWidget::onCharacterNameChanged(const QString & name)
{
    Dialogue* dialogue = qobject_cast<Dialogue*> (mAction);
    if (dialogue) {
        int i;
        for (i=0; i < mCharacters.size(); i++) {
            if (mCharacters[i]->objectName() == name) {
                dialogue->setCharacter(mCharacters[i]);
                break;
            }
        }

        //if no character with name <name> found, set just the name
        if (i == mCharacters.size())
            dialogue->setCharacterName(name);
    }
}

void DialogueEditorWidget::onTextBoxHighlighted(int index)
{
    if (index >= 0 && index < mOutputBoxes.size() && mAction && mAction->scene()) {
        mAction->scene()->highlightObject(mOutputBoxes[index]);
    }
}

void DialogueEditorWidget::onCharacterHighlighted(int index)
{
    if (index >= 0 && index < mCharacters.size() && mAction && mAction->scene()) {
        mAction->scene()->highlightObject(mCharacters[index]);
    }
}


bool DialogueEditorWidget::eventFilter(QObject *obj, QEvent *event)
{
    Scene* scene = 0;
    if (mAction)
        scene = mAction->scene();

    if ((obj == mChooseTextBoxWidget->view() || obj == mChooseCharacterWidget->view()) && event->type() == QEvent::Hide) {
       if ( scene)
           scene->highlightObject(0);
       return false;
    }

    if (obj == mChooseTextBoxWidget->view() && event->type() == QEvent::Show && scene) {
        if (mChooseTextBoxWidget->count() == 1 && ! mOutputBoxes.isEmpty())
            scene->highlightObject(mOutputBoxes.first());
    }
    else if (obj == mChooseCharacterWidget->view() && event->type() == QEvent::Show && scene) {
      if (mChooseCharacterWidget->count() == 1 && ! mCharacters.isEmpty())
           scene->highlightObject(mCharacters.first());
    }


   return false;
}

bool DialogueEditorWidget::isValidOutputBox(Object* object)
{
    if (! object)
        return false;

    QString className = object->metaObject()->className();
    if (className == "TextBox" || className == "DialogueBox")
        return true;

    return false;
}

void DialogueEditorWidget::setTextInOutputBox()
{
    Dialogue* dialogue = qobject_cast<Dialogue*> (mAction);
    if (! dialogue || ! mAction->sceneObject())
        return;

    QString text = dialogue->text();
    DialogueBox * dialogueBox = qobject_cast<DialogueBox*>(mAction->sceneObject());
    if (dialogueBox) {
        dialogueBox->setText(dialogue->characterName(), text);
    }
    else {
        TextBox* textBox = qobject_cast<TextBox*>(mAction->sceneObject());
        if (textBox) {
            textBox->setPlaceholderText(text);
        }
    }
}

void DialogueEditorWidget::onWaitOnFinishedChanged(bool state)
{
    if (mAction)
        mAction->setMouseClickOnFinish(state);
}
