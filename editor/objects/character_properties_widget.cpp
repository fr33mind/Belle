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

#include <QDebug>

CharacterPropertiesWidget::CharacterPropertiesWidget(QWidget *parent) :
    ObjectEditorWidget(parent)
{
    mCurrentCharacter = 0;
    //mNameEdit = new QLineEdit(this);
    mStatusChooser = new QComboBox(this);
    mNameColorButton = new ColorPushButton(this);
    mTextColorButton = new ColorPushButton(this);
    //mAddCharacterWidget = new AddCharacterWidget(this);


    //mAddCharacterWidget->nameEdit()->hide();
    beginGroup(tr("Character"));
    //appendRow(tr("Name"), mNameEdit);
    appendRow(tr("Default State"), mStatusChooser);
    appendRow(tr("Name Color"), mNameColorButton);
    appendRow(tr("Text Color"), mTextColorButton);
    /*WidgetGroup *widgetGroup = new WidgetGroup(Qt::Horizontal, this);
    widgetGroup->addWidget(new QLineEdit(widgetGroup), true, QSizePolicy::MinimumExpanding);
    widgetGroup->addWidget(new QPushButton(widgetGroup), true, QSizePolicy::Fixed);*/
    endGroup();

    //appendRow("Status: ", mAddCharacterWidget);
    connect(mNameColorButton, SIGNAL(colorChosen(const QColor&)), this, SLOT(onNameColorChosen(const QColor&)));
    connect(mTextColorButton, SIGNAL(colorChosen(const QColor&)), this, SLOT(onTextColorChosen(const QColor&)));
    connect(mStatusChooser, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(onStatusChosen(const QString &)));
}

void CharacterPropertiesWidget::updateData(Object *obj)
{
    ObjectEditorWidget::updateData(obj);

    mCurrentCharacter = qobject_cast<Character*>(obj);
    if (! mCurrentCharacter)
        return;

    //mNameEdit->setText(mCurrentCharacter->name());
    mStatusChooser->clear();

    QHash <QString, QString> stateToPath = mCurrentCharacter->statesToPaths();
    QList<QString> keys = stateToPath.keys();

    for (int i=0; i < keys.size(); i++)
        mStatusChooser->addItem(keys.value(i));


    mNameColorButton->setColor(mCurrentCharacter->nameColor());
    mTextColorButton->setColor(mCurrentCharacter->textColor());
    //mStatusChooser->addItem(keys.value(i));
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

void CharacterPropertiesWidget::onStatusChosen(const QString & text)
{
    if (mCurrentCharacter)
        mCurrentCharacter->setCurrentState(text);
}
