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

#include "changecolor_editor_widget.h"

#include "changecolor.h"

ChangeColorEditorWidget::ChangeColorEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mObjectsComboBox = new ObjectComboBox(this);
    mColorButton = new ColorPushButton(this);
    mOpacitySlider = new QSlider(Qt::Horizontal, this);
    mChangeObjectColorCheckBox = new QCheckBox(this);
    mChangeObjectColorCheckBox->setChecked(true);
    mChangeObjectBackgroundColorCheckBox = new QCheckBox(this);
    mOpacitySlider->setMinimum(0);
    mOpacitySlider->setMaximum(255);

    beginGroup(tr("Change color editor"));
    beginSubGroup(tr("Object"), mObjectsComboBox);
    appendRow(tr("Object"), mChangeObjectColorCheckBox);
    appendRow(tr("Object's background"), mChangeObjectBackgroundColorCheckBox);
    endGroup();
    appendRow(tr("Color"), mColorButton);
    appendRow(tr("Opacity"), mOpacitySlider);
    endGroup();
    resizeColumnToContents(0);

    connect(mObjectsComboBox, SIGNAL(objectChanged(Object*)), this, SLOT(onCurrentObjectChanged(Object*)));
    connect(mChangeObjectColorCheckBox, SIGNAL(toggled(bool)), this, SLOT(onChangeObjectColorToggled(bool)));
    connect(mChangeObjectBackgroundColorCheckBox, SIGNAL(toggled(bool)), this, SLOT(onChangeObjectBackgroundColorToggled(bool)));
    connect(mColorButton, SIGNAL(colorChosen(const QColor&)), this, SLOT(onColorChosen(const QColor&)));
    connect(mOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(onOpacityChanged(int)));

}

void ChangeColorEditorWidget::updateData(Action * action)
{
    if (action == mAction)
        return;

    ChangeColor* changeColor = qobject_cast<ChangeColor*>(action);
    if (! changeColor)
        return;

    ActionEditorWidget::updateData(action);
    mAction = 0;

    mObjectsComboBox->loadFromAction(action);
    mColorButton->setColor(changeColor->color());
    mOpacitySlider->setValue(changeColor->opacity());

    mAction = action;
}

void ChangeColorEditorWidget::onColorChosen(const QColor & color)
{
    ChangeColor* changeColor = qobject_cast<ChangeColor*>(mAction);
    if (changeColor) {
        changeColor->setColor(color);
    }
}

void ChangeColorEditorWidget::onCurrentObjectChanged(Object* object)
{
    ChangeColor* changeColor = qobject_cast<ChangeColor*>(mAction);
    if (! changeColor)
        return;

    changeColor->setSceneObject(object);
}

void ChangeColorEditorWidget::onChangeObjectColorToggled(bool checked)
{
    if (! checked && !mChangeObjectBackgroundColorCheckBox->isChecked())
        mChangeObjectColorCheckBox->setChecked(true);

    ChangeColor* changeColor = qobject_cast<ChangeColor*>(mAction);
    if(changeColor)
        changeColor->setChangeObjectColor(mChangeObjectColorCheckBox->isChecked());
}

void ChangeColorEditorWidget::onChangeObjectBackgroundColorToggled(bool checked)
{
    if (! checked && ! mChangeObjectColorCheckBox->isChecked())
        mChangeObjectBackgroundColorCheckBox->setChecked(true);

    ChangeColor* changeColor = qobject_cast<ChangeColor*>(mAction);
    if(changeColor)
        changeColor->setChangeObjectBackgroundColor(mChangeObjectBackgroundColorCheckBox->isChecked());
}

void ChangeColorEditorWidget::onOpacityChanged(int value)
{
    ChangeColor* changeColor = qobject_cast<ChangeColor*>(mAction);
    if(changeColor)
        changeColor->setOpacity(value);
}
