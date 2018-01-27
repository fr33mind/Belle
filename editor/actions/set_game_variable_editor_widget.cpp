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
#include "variablevalidator.h"

SetGameVariableEditorWidget::SetGameVariableEditorWidget(ActionEditorWidget *parent) :
    ActionEditorWidget(parent)
{
    mVariableValidator = new VariableValidator(this);
    mNumberValidator = new NumberValidator(this);

    mVariableEdit = new QLineEdit(this);
    mVariableEdit->setValidator(new VariableValidator(this));

    mOperatorChooser = new QComboBox(this);
    mOperatorChooser->addItem(tr("Assign"));
    mOperatorChooser->addItem(tr("Add"));
    mOperatorChooser->addItem(tr("Subtract"));
    mOperatorChooser->addItem(tr("Multiply"));
    mOperatorChooser->addItem(tr("Divide"));
    mOperatorChooser->addItem(tr("Append"));

    mValueTypeChooser = new QComboBox(this);
    mValueTypeChooser->addItem(tr("Value"));
    mValueTypeChooser->addItem(tr("Variable"));

    mValueEdit = new QLineEdit(this);

    connect(mVariableEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onVariableEdited(const QString&)));
    connect(mOperatorChooser, SIGNAL(currentIndexChanged(int)), this, SLOT(onOperatorChanged(int)));
    connect(mValueTypeChooser, SIGNAL(currentIndexChanged(int)), this, SLOT(onValueTypeChanged(int)));
    connect(mValueEdit, SIGNAL(textChanged(const QString&)), this, SLOT(onValueChanged(const QString&)));

    beginGroup(tr("Game Variable"));
    appendRow(tr("Variable"), mVariableEdit);
    appendRow(tr("Operator"), mOperatorChooser);
    appendRow(tr("Value Type"), mValueTypeChooser);
    appendRow(tr("Value"), mValueEdit, "ValueEdit");
    endGroup();

    mValueEditLabelItem = findItemData("ValueEdit");

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
    mValueTypeChooser->setCurrentIndex(static_cast<int>(setGameVariable->valueType()));
    updateValueEditType(setGameVariable);
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

    updateValueEditType(setGameVariable);
}

void SetGameVariableEditorWidget::onValueTypeChanged(int index)
{
    SetGameVariable* setGameVariable = qobject_cast<SetGameVariable*>(mGameObject);
    if (setGameVariable)
        setGameVariable->setValueType(static_cast<SetGameVariable::ValueType>(index));

    updateValueEditType(setGameVariable);
}

void SetGameVariableEditorWidget::onValueChanged(const QString & text)
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

void SetGameVariableEditorWidget::updateValueEditType(SetGameVariable * setGameVariable)
{
    if (!setGameVariable)
        return;

    int index = setGameVariable->operatorIndex();
    SetGameVariable::ValueType type = setGameVariable->valueType();
    QString label;

    if (type == SetGameVariable::Value && index >= 1 && index <= 4) {
        mValueEdit->setValidator(mNumberValidator);
        if (mNumberValidator->validate(mValueEdit->text()) != QValidator::Acceptable) {
            mValueEdit->clear();
        }

        label = tr("Number");
    }
    else if (type == SetGameVariable::Variable) {
        mValueEdit->setValidator(mVariableValidator);
        if (mVariableValidator->validate(mValueEdit->text()) != QValidator::Acceptable) {
            mValueEdit->clear();
        }

        label = tr("Variable");
    }
    else {
        mValueEdit->setValidator(0);
        label = tr("Value");
    }

    mValueEditLabelItem->setText(label);
}
