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

#include "get_user_input_editor_widget.h"

#include <QLineEdit>

#include "get_user_input.h"
#include "variablevalidator.h"

GetUserInputEditorWidget::GetUserInputEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mMessageEdit = new QLineEdit(this);
    connect(mMessageEdit, SIGNAL(textEdited(const QString &)), this, SLOT(onMessageChanged(const QString &)));

    mVariableEdit = new QLineEdit(this);
    mVariableEdit->setValidator(new VariableValidator(this));
    connect(mVariableEdit, SIGNAL(textEdited(const QString &)), this, SLOT(onVariableChanged(const QString &)));

    mDefaultValueEdit= new QLineEdit(this);
    connect(mDefaultValueEdit, SIGNAL(textEdited(const QString &)), this, SLOT(onDefaultValueChanged(const QString &)));

    beginGroup(tr("Get user input editor"));
    appendRow(tr("Message"), mMessageEdit);
    appendRow(tr("Variable"), mVariableEdit);
    appendRow(tr("Default Value"), mDefaultValueEdit);
    endGroup();

    resizeColumnToContents(0);
}

void GetUserInputEditorWidget::updateData(GameObject* action)
{
    ActionEditorWidget::updateData(action);
    GetUserInput* userInput = qobject_cast<GetUserInput*>(action);
    if (! userInput)
        return;

    mMessageEdit->setText(userInput->message());
    mVariableEdit->setText(userInput->variable());
    mDefaultValueEdit->setText(userInput->defaultValue());
}

void GetUserInputEditorWidget::onMessageChanged(const QString & value)
{
    GetUserInput* userInput = qobject_cast<GetUserInput*>(mGameObject);
    if (userInput)
        userInput->setMessage(value);
}

void GetUserInputEditorWidget::onVariableChanged(const QString & value)
{
    GetUserInput* userInput = qobject_cast<GetUserInput*>(mGameObject);
    if (userInput)
        userInput->setVariable(value);
}

void GetUserInputEditorWidget::onDefaultValueChanged(const QString & value)
{
    GetUserInput* userInput = qobject_cast<GetUserInput*>(mGameObject);
    if (userInput)
        userInput->setDefaultValue(value);
}
