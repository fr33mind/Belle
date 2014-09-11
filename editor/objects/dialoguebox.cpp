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

#include "dialoguebox.h"

#include <QDebug>
#include "scene.h"
#include "resource_manager.h"

DialogueBox::DialogueBox(QObject *parent, const QString& name) :
    ObjectGroup(parent, name)
{
    init();

    setY(Scene::height()- (Scene::height()/3));

    TextBox *speakerTextBox = new TextBox("", this);
    speakerTextBox->setPlaceholderText("Narrator");
    speakerTextBox->setObjectName("speakerTextBox");
    speakerTextBox->setEditableName(false);
    speakerTextBox->setHeight(30);
    this->append(speakerTextBox);

    QString text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec a diam lectus.";
    TextBox *dialogueTextBox = new TextBox("", this);
    dialogueTextBox->setPlaceholderText(text);
    dialogueTextBox->setEditableName(false);
    dialogueTextBox->setObjectName("dialogueTextBox");
    dialogueTextBox->setHeight(Scene::height()/3);
    this->append(dialogueTextBox);
}

DialogueBox::DialogueBox(const QVariantMap& data, QObject* parent):
    ObjectGroup(data, parent)
{
    init();
}

void DialogueBox::init()
{
    setType("DialogueBox");
}

DialogueBox::~DialogueBox()
{
}

void DialogueBox::setText(const QString & speaker, const QString & text)
{
    blockSignals(true); //avoid calling dataChanged() multiple times
    setSpeakerName(speaker);
    setText(text);
    blockSignals(false);

    emit dataChanged();
}

void DialogueBox::setText(const QString & text)
{
    TextBox *textBox = this->textBox("dialogueTextBox");
    if (textBox) {
        textBox->setPlaceholderText(text);
        emit dataChanged();
    }
}

void DialogueBox::setSpeakerName(const QString & speaker)
{
    TextBox *textBox = this->textBox("speakerTextBox");
    if (textBox) {
        textBox->setPlaceholderText(speaker);
        emit dataChanged();
    }
}

void DialogueBox::paint(QPainter & painter)
{

    ObjectGroup::paint(painter);

    /*painter.save();
    QFontMetrics fontMetrics(painter.font());
    //mSpeakerspeakerTextBox->setWidth(fontMetrics.width(mSpeakerspeakerTextBox->text()));
    //mSpeakerspeakerTextBox->setHeight(fontMetrics.height());
    QPen pen(mSpeakerColor);
    painter.setPen(pen);
    painter.drawText(mSceneRect, mSpeakerspeakerTextBox->text());
    QRectF dialogueRect = mSceneRect;
    dialogueRect.setY(dialogueRect.y()+fontMetrics.height());
    pen.setColor(textColor());
    painter.setPen(pen);
    painter.drawText(dialogueRect, text());
    painter.restore();*/
}

void DialogueBox::move(int x, int y)
{
    ObjectGroup::move(x, y);
}

void DialogueBox::setSpeakerNameColor(const QColor & color)
{
    TextBox *textBox = this->textBox("speakerTextBox");
    if (textBox) {
        textBox->setTextColor(color);
        emit dataChanged();
    }
}

QVariantMap DialogueBox::toJsonObject(bool internal)
{
    return ObjectGroup::toJsonObject(internal);
}

TextBox* DialogueBox::textBox(const QString& name)
{
    Object * obj = this->object(name);
    if (obj)
        return qobject_cast<TextBox*>(obj);
    return 0;
}

void DialogueBox::setTextColor(const QColor & color)
{
    TextBox *textBox = this->textBox("dialogueTextBox");
    if (textBox)
        textBox->setTextColor(color);
}

void DialogueBox::activateDefaultTextColor()
{
    TextBox *textBox = this->textBox("dialogueTextBox");
    if (textBox)
        textBox->activateDefaultTextColor();
    textBox = this->textBox("speakerTextBox");
    if (textBox)
        textBox->activateDefaultTextColor();
}

/*DialogueEditorWidget* DialogueBox::dialogueEditorWidget()
{
    return mEditorWidget;
}

ObjectEditorWidget* DialogueBox::editorWidget()
{
    return mEditorWidget;
}

void DialogueBox::setDialogueEditorWidget(DialogueEditorWidget* widget)
{
    if (! widget)
        return;

    if (mEditorWidget)
        mEditorWidget->deleteLater();

    mEditorWidget = widget;
}
*/
