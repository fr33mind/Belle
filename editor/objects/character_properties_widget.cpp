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
    mCurrentCharacter = 0;
    //mNameEdit = new QLineEdit(this);

    QWidget *widget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(widget);
    mStateChooser = new QComboBox(this);
    mStateChooser->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QPushButton* stateEditButton = new QPushButton(QIcon(":/media/object-edit.png"), "", widget);
    stateEditButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    layout->addWidget(mStateChooser);
    layout->addWidget(stateEditButton);
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

void CharacterPropertiesWidget::updateData(Object *obj)
{
    ObjectEditorWidget::updateData(obj);

    mCurrentCharacter = qobject_cast<Character*>(obj);
    if (! mCurrentCharacter)
        return;

    //mNameEdit->setText(mCurrentCharacter->name());
    mNameColorButton->setColor(mCurrentCharacter->nameColor());
    mTextColorButton->setColor(mCurrentCharacter->textColor());
    updateCharacterStates();
}

void CharacterPropertiesWidget::onNameColorChosen(const QColor & color)
{
    if (mCurrentCharacter)
        mCurrentCharacter->setNameColor(color);
}

void CharacterPropertiesWidget::onTextColorChosen(const QColor & color)
{
    if (mCurrentCharacter)
        mCurrentCharacter->setTextColor(color);
}

void CharacterPropertiesWidget::onStateChanged(const QString & text)
{
    if (mCurrentCharacter)
        mCurrentCharacter->setCurrentState(text);
}

void CharacterPropertiesWidget::updateCharacterStates()
{
    if (! mCurrentCharacter)
        return;

    QHash <QString, QString> stateToPath = mCurrentCharacter->statesToPaths();
    QList<QString> keys = stateToPath.keys();
    mStateChooser->clear();
    int curr = -1;
    QString currState = mCurrentCharacter->currentState();
    for (int i=0; i < keys.size(); i++) {
        if (keys[i] == currState)
            curr = i;
        mStateChooser->addItem(keys.value(i));
    }

    mStateChooser->setCurrentIndex(curr);
}

void CharacterPropertiesWidget::editCharacterStates()
{
    if (! mCurrentCharacter)
        return;

    AddCharacterDialog* dialog = new AddCharacterDialog(mCurrentCharacter);
    bool accepted = dialog->exec();
    if (accepted) {
        mCurrentCharacter->setStates(dialog->statesAndImagePaths());
        mCurrentCharacter->setName(dialog->name());
    }

    dialog->deleteLater();
    updateCharacterStates();
}
