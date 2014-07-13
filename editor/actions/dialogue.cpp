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

#include "dialogue.h"

#include <QTextCodec>
#include <QDebug>

static DialogueEditorWidget *mEditorWidget = 0;
ActionInfo Dialogue::Info;

Dialogue::Dialogue(QObject *parent) :
    Action(parent)
{
    init();
}

Dialogue::Dialogue(const QVariantMap & data, QObject *parent):
    Action(data, parent)
{
    init();

    if (data.contains("character") && data.value("character").type() == QVariant::String) {
        mCharacter = qobject_cast<Character*>(ResourceManager::resource(data.value("character").toString()));

        if (mCharacter)
            mCharacterName = mCharacter->objectName();
        else
            mCharacterName = data.value("character").toString();
    }

    if (data.contains("text") && data.value("text").type() == QVariant::String) {
            setText(data.value("text").toString());
    }

    setMouseClickOnFinish(data.contains("wait"));
}

void Dialogue::init()
{
    setType(Info.type);
    setName(Info.name);
    setIcon(Info.icon);
    mCharacter = 0;
    mText = "";
    setMouseClickOnFinish(true);
}

void Dialogue::setDialogueEditorWidget(DialogueEditorWidget * widget)
{
   mEditorWidget = widget;
}

DialogueEditorWidget* Dialogue::dialogueEditorWidget()
{
   return mEditorWidget;
}

ActionEditorWidget* Dialogue::editorWidget()
{
    return mEditorWidget;
}

void Dialogue::setCharacter(Character *character)
{
    if (mCharacter)
        mCharacter->disconnect(this);

    mCharacter = character;

    if (mCharacter)
        connect(mCharacter, SIGNAL(destroyed()), this, SLOT(onCharacterDestroyed()));

    if (character) {
        this->blockSignals(true); //avoid calling dataChanged() twice
        setCharacterName(character->objectName());
        this->blockSignals(false);
    }

    //update dialoguebox or textbox
    if (sceneObject() && character) {
        DialogueBox* obj = qobject_cast<DialogueBox*>(sceneObject());
        if (obj) {
            //dialogueBox->setSpeakerName(name); // has been called in setCharacterName(..)
            obj->setSpeakerColor(character->nameColor());
            obj->setTextColor(character->textColor());
        }
        else {
            TextBox* textObj = qobject_cast<TextBox*>(sceneObject());
            if (textObj)
                textObj->setTextColor(character->textColor());
        }
    }

    emit dataChanged();
}

Character* Dialogue::character()
{
    return mCharacter;
}

void Dialogue::setCharacterName(const QString & name)
{
    if (mCharacter && mCharacter->objectName() != name)
        mCharacter = 0;
    mCharacterName = name;

    //update dialoguebox if any
    if (sceneObject()) {
        DialogueBox* dialogueBox = qobject_cast<DialogueBox*>(sceneObject());
        if (dialogueBox)
            dialogueBox->setSpeakerName(name);
    }

    emit dataChanged();
}

QString Dialogue::characterName() const
{
    //the character's name can change after being assigned to this action
    //so mCharacterName could be outdated.
    if (mCharacter)
        return mCharacter->objectName();
    return mCharacterName;
}

void Dialogue::setText(const QString & text)
{
    mText = text;

    TextBox* textObj = qobject_cast<TextBox*>(sceneObject());
    if (textObj) {
        textObj->setPlaceholderText(text);
    }
    else { //if dialogueBox
        DialogueBox* dialogueBox = qobject_cast<DialogueBox*>(sceneObject());
        if (dialogueBox)
            dialogueBox->setText(text);
    }

    emit dataChanged();
}

QString Dialogue::text()
{
    return mText;
}

void Dialogue::paint(const QPainter & painter)
{
}

QString Dialogue::displayText() const
{
    QString text("");
    if (! characterName().isEmpty())
        text += characterName() + ": ";


    text += '"' + mText + '"';

    return text;
}

QVariantMap Dialogue::toJsonObject()
{
    QVariantMap object = Action::toJsonObject();

    if (mCharacter)
        object.insert("character", mCharacter->objectName());
    else if (! mCharacterName.isEmpty())
        object.insert("character", mCharacterName);

    object.insert("text", mText);
    return object;
}

void Dialogue::onCharacterDestroyed()
{
    mCharacter = 0;
}
