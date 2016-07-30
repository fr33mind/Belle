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

#include "set_game_variable_editor_widget.h"

SetGameVariableEditorWidget::SetGameVariableEditorWidget(ActionEditorWidget *parent) :
    ActionEditorWidget(parent)
{
    mVariableEdit = new QLineEdit(this);
    QRegExpValidator* validator = new QRegExpValidator(QRegExp("^[a-zA-Z_]+[a-zA-Z_0-9]*$"), this);
    mVariableEdit->setValidator(validator);

    mOperatorChooser = new QComboBox(this);
    mOperatorChooser->addItem(tr("Assign"));
    mOperatorChooser->addItem(tr("Add"));
    mOperatorChooser->addItem(tr("Subtract"));
    mOperatorChooser->addItem(tr("Multiply"));
    mOperatorChooser->addItem(tr("Divide"));
    mOperatorChooser->addItem(tr("Append"));

    mValueEdit = new QLineEdit(this);

    connect(mVariableEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onVariableEdited(const QString&)));
    connect(mOperatorChooser, SIGNAL(currentIndexChanged(int)), this, SLOT(onOperatorChanged(int)));
    connect(mValueEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onValueEdited(const QString&)));

    beginGroup(tr("Game Variable"));
    appendRow(tr("Variable"), mVariableEdit);
    appendRow(tr("Operator"), mOperatorChooser);
    appendRow(tr("Value"), mValueEdit);
    endGroup();

    resizeColumnToContents(0);
}

void  SetGameVariableEditorWidget::updateData(GameObject* action)
{
    ActionEditorWidget::updateData(action);
    SetGameVariable* setGameVariable = qobject_cast<SetGameVariable*>(action);
    if (! setGameVariable)
        return;

    mVariableEdit->setText(setGameVariable->variable());
    mOperatorChooser->setCurrentIndex(setGameVariable->operatorIndex());
    mValueEdit->setText(setGameVariable->value());
}

void SetGameVariableEditorWidget::onVariableEdited(const QString & text)
{
    SetGameVariable* setGameVariable = qobject_cast<SetGameVariable*>(mGameObject);
    if (setGameVariable)
        setGameVariable->setVariable(text);
}

void SetGameVariableEditorWidget::onOperatorChanged(int index)
{
    SetGameVariable* setGameVariable = qobject_cast<SetGameVariable*>(mGameObject);
    if (setGameVariable)
        setGameVariable->setOperatorIndex(index);
}

void SetGameVariableEditorWidget::onValueEdited(const QString & text)
{
    SetGameVariable* setGameVariable = qobject_cast<SetGameVariable*>(mGameObject);
    if (setGameVariable)
        setGameVariable->setValue(text);
}

QStringList SetGameVariableEditorWidget::operatorsText()
{
    QStringList operators;
    for(int i=0; i < mOperatorChooser->count(); i++)
        operators << mOperatorChooser->itemText(i);

    return operators;
}
