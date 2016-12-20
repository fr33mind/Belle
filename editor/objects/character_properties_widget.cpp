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

#include "character_properties_widget.h"
#include "add_character_dialog.h"

#include <QDebug>
#include <QHBoxLayout>

CharacterPropertiesWidget::CharacterPropertiesWidget(QWidget *parent) :
    ObjectEditorWidget(parent)
{
    //mNameEdit = new QLineEdit(this);

    QWidget *widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    mStateChooser = new QComboBox(this);
    mStateChooser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QPushButton* stateEditButton = new QPushButton(QIcon(":/media/object-edit.png"), "", widget);
    stateEditButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    layout->addWidget(mStateChooser);
    layout->addWidget(stateEditButton);
    layout->setContentsMargins(0, 1, 1, 1);
    mNameColorButton = new ColorPushButton(this);
    mTextColorButton = new ColorPushButton(this);
    //mAddCharacterWidget = new AddCharacterWidget(this);

    //mAddCharacterWidget->nameEdit()->hide();
    beginGroup(tr("Character"));
    //appendRow(tr("Name"), mNameEdit);
    appendRow(tr("Default State"), widget);
    appendRow(tr("Name Color"), mNameColorButton);
    appendRow(tr("Text Color"), mTextColorButton);
    /*WidgetGroup *widgetGroup = new WidgetGroup(Qt::Horizontal, this);
    widgetGroup->addWidget(new QLineEdit(widgetGroup), true, QSizePolicy::MinimumExpanding);
    widgetGroup->addWidget(new QPushButton(widgetGroup), true, QSizePolicy::Fixed);*/
    endGroup();

    //appendRow("Status: ", mAddCharacterWidget);
    connect(mNameColorButton, SIGNAL(colorChosen(const QColor&)), this, SLOT(onNameColorChosen(const QColor&)));
    connect(mTextColorButton, SIGNAL(colorChosen(const QColor&)), this, SLOT(onTextColorChosen(const QColor&)));
    connect(mStateChooser, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onStateChanged(const QString &)));
    connect(stateEditButton, SIGNAL(clicked()), this, SLOT(editCharacterStates()));
}

void CharacterPropertiesWidget::updateData(GameObject* obj)
{
    ObjectEditorWidget::updateData(obj);
    Character* character = qobject_cast<Character*>(obj);
    if (! character)
        return;

    //mNameEdit->setText(mCurrentCharacter->name());
    mNameColorButton->setColor(character->nameColor());
    mTextColorButton->setColor(character->textColor());
    updateCharacterStates(character);
}

void CharacterPropertiesWidget::onNameColorChosen(const QColor & color)
{
    Character* character = qobject_cast<Character*>(mGameObject);
    if (character)
        character->setNameColor(color);
}

void CharacterPropertiesWidget::onTextColorChosen(const QColor & color)
{
    Character* character = qobject_cast<Character*>(mGameObject);
    if (character)
        character->setTextColor(color);
}

void CharacterPropertiesWidget::onStateChanged(const QString & text)
{
    Character* character = qobject_cast<Character*>(mGameObject);
    if (character)
        character->setCurrentState(text);
}

void CharacterPropertiesWidget::updateCharacterStates(Character* character)
{
    if (! character)
        return;

    QHash <QString, QString> stateToPath = character->statesToPaths();
    QList<QString> keys = stateToPath.keys();
    mStateChooser->clear();
    int curr = -1;
    QString currState = character->currentState();
    for (int i=0; i < keys.size(); i++) {
        if (keys[i] == currState)
            curr = i;
        mStateChooser->addItem(keys.value(i));
    }

    mStateChooser->setCurrentIndex(curr);
}

void CharacterPropertiesWidget::editCharacterStates()
{
    Character* character = qobject_cast<Character*>(mGameObject);
    if (! character)
        return;

    AddCharacterDialog* dialog = new AddCharacterDialog(character);
    bool accepted = dialog->exec();
    if (accepted) {
        character->setStates(dialog->statesAndImagePaths());
        character->setName(dialog->name());
    }

    dialog->deleteLater();
    updateCharacterStates(character);
}
