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

#include "change_game_variable_editor_widget.h"

#include <QDebug>

ChangeGameVariableEditorWidget::ChangeGameVariableEditorWidget(ActionEditorWidget *parent) :
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

void  ChangeGameVariableEditorWidget::updateData(Action * action)
{
    if (action == mAction)
        return;

    ChangeGameVariable* changeGameVariable = qobject_cast<ChangeGameVariable*>(action);
    if (! changeGameVariable)
        return;

    ActionEditorWidget::updateData(action);
    mAction = 0;

    mVariableEdit->setText(changeGameVariable->variable());
    mOperatorChooser->setCurrentIndex(changeGameVariable->operatorIndex());
    mValueEdit->setText(changeGameVariable->value());

    mAction = action;
}

void ChangeGameVariableEditorWidget::onVariableEdited(const QString & text)
{
    ChangeGameVariable* changeGameVariable = qobject_cast<ChangeGameVariable*>(mAction);
    if (changeGameVariable)
        changeGameVariable->setVariable(text);
}

void ChangeGameVariableEditorWidget::onOperatorChanged(int index)
{
    ChangeGameVariable* changeGameVariable = qobject_cast<ChangeGameVariable*>(mAction);
    if (changeGameVariable)
        changeGameVariable->setOperatorIndex(index);
}

void ChangeGameVariableEditorWidget::onValueEdited(const QString & text)
{
    ChangeGameVariable* changeGameVariable = qobject_cast<ChangeGameVariable*>(mAction);
    if (changeGameVariable)
        changeGameVariable->setValue(text);
}

QStringList ChangeGameVariableEditorWidget::operatorsText()
{
    QStringList operators;
    for(int i=0; i < mOperatorChooser->count(); i++)
        operators << mOperatorChooser->itemText(i);

    return operators;
}
