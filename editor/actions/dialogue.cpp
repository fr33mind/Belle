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

Dialogue::Dialogue(QObject *parent) :
    Action(parent)
{
    init();
}

Dialogue::Dialogue(const QVariantMap & data, QObject *parent):
    Action(data, parent)
{
    init();
    loadInternal(data);
}

void Dialogue::init()
{
    setType(GameObjectMetaType::Dialogue);
    mCharacter = 0;
    mText = "";
    mAppend = false;
    setMouseClickOnFinish(true);
}

void Dialogue::loadData(const QVariantMap & data, bool internal)
{
    if (!internal)
        Action::loadData(data, internal);

    if (data.contains("character") && data.value("character").type() == QVariant::String) {
        Scene* scene = this->scene();
        Character* character = 0;
        QString charName = data.value("character").toString();
        if (scene)
            character = qobject_cast<Character*>(scene->object(charName));
        setCharacter(character);

        if (!mCharacter)
            setCharacterName(charName);
    }

    if (data.contains("text") && data.value("text").type() == QVariant::String) {
        setText(data.value("text").toString());
    }

    if (data.contains("append") && data.value("append").type() == QVariant::Bool) {
        setAppend(data.value("append").toBool());
    }
}

void Dialogue::setCharacter(Character *character)
{
    if (mCharacter == character)
        return;

    removeCharacter();
    mCharacter = character;
    mCharacterName = "";

    if (mCharacter) {
        connect(mCharacter, SIGNAL(destroyed()), this, SLOT(onCharacterDestroyed()));
        mCharacterName = mCharacter->name();
    }

    //update DialogueBox or TextBox
    updateTextBox();
    notify("character", characterName());
}

Character* Dialogue::character()
{
    return mCharacter;
}

void Dialogue::setCharacterName(const QString & name)
{
    if (mCharacterName == name)
        return;

    removeCharacter();
    mCharacterName = name;

    //update dialoguebox if any
    updateTextBox();
    notify("character", characterName());
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
    updateTextBox();
    notify("text", mText);
}

QString Dialogue::text()
{
    return mText;
}

QString Dialogue::displayText() const
{
    QString text("");
    if (! characterName().isEmpty())
        text += characterName() + ": ";


    text += '"' + mText + '"';

    return text;
}

QVariantMap Dialogue::toJsonObject(bool internal) const
{
    QVariantMap object = Action::toJsonObject(internal);

    if (mCharacter)
        object.insert("character", mCharacter->objectName());
    else if (! mCharacterName.isEmpty())
        object.insert("character", mCharacterName);

    if (mAppend)
        object.insert("append", mAppend);

    object.insert("text", mText);
    return object;
}

void Dialogue::onCharacterDestroyed()
{
    mCharacter = 0;
}

void Dialogue::updateTextBox()
{
    if (!isActive())
        return;

    Object* object = sceneObject();
    if (! object)
        return;

    QColor textColor, nameColor;
    QString name = mCharacterName;

    if (mCharacter) {
        textColor = mCharacter->textColor();
        nameColor = mCharacter->nameColor();
        name = mCharacter->name();
    }

    DialogueBox* dialogueBox = qobject_cast<DialogueBox*>(object);
    if (dialogueBox) {
        dialogueBox->setSpeakerName(name);
        dialogueBox->setText(mText);
        dialogueBox->setTextColor(textColor);
        dialogueBox->setSpeakerNameColor(nameColor);
    }
    else {
        TextBox* textBox = qobject_cast<TextBox*>(object);
        if (textBox) {
            textBox->setPlaceholderTextColor(textColor);
            textBox->setPlaceholderText(mText);
        }
    }
}

void Dialogue::restoreTextBox()
{
    if (!isActive())
        return;

    Object* object = sceneObject();
    if (! object)
        return;

    DialogueBox* dialogueBox = qobject_cast<DialogueBox*>(object);
    if (dialogueBox) {
       dialogueBox->setText("", "");
    }
    else {
        TextBox* textBox = qobject_cast<TextBox*>(object);
        if (textBox) {
            textBox->setPlaceholderText("");
        }
    }
}

void Dialogue::loadSceneObject()
{
    Action::loadSceneObject();
    updateTextBox();
}

void Dialogue::restoreSceneObject()
{
    Action::restoreSceneObject();
    restoreTextBox();
}

bool Dialogue::append() const
{
    return mAppend;
}

void Dialogue::setAppend(bool append)
{
    mAppend = append;
    notify("append", mAppend);
}

void Dialogue::removeCharacter()
{
    if (mCharacter) {
        mCharacter->disconnect(this);
        mCharacter = 0;
    }
}
