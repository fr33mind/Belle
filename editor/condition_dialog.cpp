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

#include "condition_dialog.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QTreeWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QApplication>

#include "condition_widget.h"
#include "utils.h"

ConditionDialog::ConditionDialog(ComplexCondition* condition, QWidget *parent) :
    QDialog(parent)
{
    mCondition = condition;
    mEditingCondition = 0;
    mVariableValidator = new VariableValidator(this);
    QVBoxLayout* vlayout1 = 0, * vlayout2 = 0;
    setWindowTitle(tr("Edit Condition"));
    QVBoxLayout* mainLayout = new QVBoxLayout;
    if (this->layout())
        this->layout()->deleteLater();

    setLayout(mainLayout);

    mConditionWidget = new ConditionWidget(condition, this);
    connect(mConditionWidget, SIGNAL(selectedConditionChanged(AbstractCondition*)), this, SLOT(setEditingCondition(AbstractCondition*)));

    //Create logical operator ComboBox
    mLogicalOperators = new QComboBox(this);
    mLogicalOperators->addItem(tr("And"), ConditionLogicalOperator::And);
    mLogicalOperators->addItem(tr("Or"), ConditionLogicalOperator::Or);
    mLogicalOperators->setVisible(false);

    //edit variable widget
    mLeftMemberEdit = new QLineEdit(this);
    mLeftMemberEdit->setObjectName("variableEditor");
    setLineEditType(mLeftMemberEdit, Variable);
    connect(mLeftMemberEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onLeftMemberEdited(const QString&)));

    //data type combobox
    mDataType1Chooser = new QComboBox(this);
    mDataType1Chooser->setObjectName("type1");
    mDataType1Chooser->addItem(tr("Value"), Value);
    mDataType1Chooser->addItem(tr("Variable"), Variable);
    mDataType1Chooser->setCurrentIndex(1);
    connect(mDataType1Chooser, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypeChanged(int)));

    mOperationsComboBox = new QComboBox(this);
    QMapIterator<int, QString> it(ConditionOperation::operations());
    while(it.hasNext()) {
        it.next();
        mOperationsComboBox->addItem(it.value(), it.key());
    }
    connect(mOperationsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentOperatorChanged(int)));

    mRightMemberEdit = new QLineEdit(this);
    mRightMemberEdit->setObjectName("valueEditor");
    setLineEditType(mRightMemberEdit, Value);
    connect(mRightMemberEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onRightMemberEdited(const QString&)));

    //data type combobox
    mDataType2Chooser = new QComboBox(this);
    mDataType2Chooser->setObjectName("type2");
    mDataType2Chooser->addItem(tr("Value"), Value);
    mDataType2Chooser->addItem(tr("Variable"), Variable);
    connect(mDataType2Chooser, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypeChanged(int)));

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    mOkButton = buttonBox->button(QDialogButtonBox::Ok);

    //create add button
    mAddIcon = QIcon(":/media/add.png");
    mAddButton = new QPushButton(mAddIcon, "", this);
    mAddButton->setEnabled(false);
    connect(mAddButton, SIGNAL(clicked()), this, SLOT(onAddClicked()));

    //create first vertical layout
    vlayout1 = new QVBoxLayout();
    vlayout1->addWidget(mLeftMemberEdit);
    vlayout1->addWidget(mDataType1Chooser);

    //create second vertical layout
    vlayout2 = new QVBoxLayout();
    vlayout2->addWidget(mRightMemberEdit);
    vlayout2->addWidget(mDataType2Chooser);

    //add all widgets and layouts to parent horizontal layout
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(mLogicalOperators);
    hlayout->addLayout(vlayout1);
    hlayout->addWidget(mOperationsComboBox);
    hlayout->addLayout(vlayout2);
    hlayout->addWidget(mAddButton);

    //add all widgets and layouts to main layout
    mainLayout->addLayout(hlayout);
    mainLayout->addWidget(mConditionWidget);
    mainLayout->addWidget(buttonBox);

    if (mConditionWidget->model()->rowCount())
        mLogicalOperators->setVisible(true);
}

void ConditionDialog::onAddClicked()
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(mConditionWidget->model());
    if (! model || !mCondition)
        return;

    ConditionTokenMetaType::Type type = (ConditionTokenMetaType::Type) mDataType1Chooser->currentData().toInt();
    SimpleConditionToken left(type, mLeftMemberEdit->text());
    SimpleConditionToken right;
    ConditionOperation::Type op = (ConditionOperation::Type) mOperationsComboBox->currentData().toInt();
    if (mRightMemberEdit->isVisible()) {
        type = (ConditionTokenMetaType::Type) mDataType2Chooser->currentData().toInt();
        right = SimpleConditionToken(type, mRightMemberEdit->text());
    }

    if (editMode() && mEditingCondition) {
        if (mLogicalOperators->isVisible()) {
            ConditionLogicalOperator::Type lop = (ConditionLogicalOperator::Type) mLogicalOperators->currentData().toInt();
            ConditionToken* token = editingLogicalOperator();
            if (token)
                token->setValue(lop);
        }

        mEditingCondition->setLeftOperand(left);
        mEditingCondition->setOperation(op);
        mEditingCondition->setRightOperand(right);
        mConditionWidget->reload();
    }
    else {
        SimpleCondition* condition = new SimpleCondition(left, op, right);
        if (mLogicalOperators->isVisible()) {
            ConditionLogicalOperator::Type lop = (ConditionLogicalOperator::Type) mLogicalOperators->currentData().toInt();
            mConditionWidget->appendCondition(lop, condition);
        }
        else {
            mConditionWidget->appendCondition(condition);
        }

        if (mLogicalOperators->isHidden())
            mLogicalOperators->setVisible(true);

        reset();
    }
}

void ConditionDialog::onCurrentOperatorChanged(int index)
{
    if (index >= mOperationsComboBox->count()-4){
        mRightMemberEdit->hide();
        mDataType2Chooser->hide();
        mDataType1Chooser->setCurrentIndex(1); //set "variable"
        mDataType1Chooser->setEnabled(false);
    }
    else {
        mRightMemberEdit->show();
        mDataType2Chooser->show();
        if (! mDataType1Chooser->isEnabled())
            mDataType1Chooser->setEnabled(true);
        if (! mDataType2Chooser->isEnabled())
            mDataType2Chooser->setEnabled(true);
    }

    onLeftMemberEdited(mLeftMemberEdit->text());
}

void ConditionDialog::onLeftMemberEdited(const QString & text)
{
    if (text.isEmpty())
        mAddButton->setEnabled(false);
    else if (! mRightMemberEdit->isVisible() || (mRightMemberEdit->isVisible() && ! mRightMemberEdit->text().isEmpty()))
        mAddButton->setEnabled(true);
    else
        mAddButton->setEnabled(false);

    mAddButton->setDefault(mAddButton->isEnabled());
}

void ConditionDialog::onRightMemberEdited(const QString & text)
{
    if (mRightMemberEdit->isVisible()) {
        if (text.isEmpty() || mLeftMemberEdit->text().isEmpty())
            mAddButton->setEnabled(false);
        else
            mAddButton->setEnabled(true);

    }
    else if (! mLeftMemberEdit->text().isEmpty()){
         mAddButton->setEnabled(true);
    }
    else
         mAddButton->setEnabled(false);

    mAddButton->setDefault(mAddButton->isEnabled());
}

void ConditionDialog::onTypeChanged(int index)
{
    if (index < 0)
        return;

    QString objName = sender()->objectName();
    QComboBox* senderComboBox = static_cast<QComboBox*>(sender());
    int type = senderComboBox->itemData(index).toInt();
    QLineEdit* lineEdit = 0;

    if (objName == "type1")
        lineEdit = mLeftMemberEdit;
    else
        lineEdit = mRightMemberEdit;

    setLineEditType(lineEdit, (LineEditType) type);
}

void ConditionDialog::setLineEditType(QLineEdit * lineEdit, LineEditType type)
{
    if (!lineEdit)
        return;

    bool match = false;

    switch (type) {
        case Variable:
            lineEdit->setValidator(mVariableValidator);
            lineEdit->setPlaceholderText(tr("Variable"));
            if (mVariableValidator->validate(lineEdit->text()) == QValidator::Invalid)
                lineEdit->clear();
            break;
        case Value:
            lineEdit->setValidator(0);
            lineEdit->setPlaceholderText(tr("Value"));
            break;
        default: break;
    }
}

void ConditionDialog::setDataType(QComboBox * combobox, ConditionTokenMetaType::Type type)
{
    int index = combobox->findData(QVariant(static_cast<int>(type)));
    combobox->setCurrentIndex(index);
}

void ConditionDialog::setCurrentOperation(ConditionOperation::Type type)
{
    int index = mOperationsComboBox->findData(QVariant(static_cast<int>(type)));
    mOperationsComboBox->setCurrentIndex(index);
}

void ConditionDialog::setCurrentLogicalOperator(int type)
{
    int index = mLogicalOperators->findData(QVariant(type));
    mLogicalOperators->setCurrentIndex(index);
}

void ConditionDialog::setCurrentLogicalOperator(ConditionLogicalOperator::Type type)
{
    setCurrentLogicalOperator(static_cast<int>(type));
}

void ConditionDialog::reset()
{
    mEditingCondition = 0;
    setEditMode(false);

    if (mCondition && !mCondition->isEmpty()) {
        mLogicalOperators->setVisible(true);
        setCurrentLogicalOperator(ConditionLogicalOperator::And);
    }
    else
        mLogicalOperators->setVisible(false);

    mLeftMemberEdit->setText("");
    mRightMemberEdit->setText("");
    mOperationsComboBox->setCurrentIndex(0);
    setDataType(mDataType1Chooser, ConditionTokenMetaType::Variable);
    setDataType(mDataType2Chooser, ConditionTokenMetaType::Value);
    mAddButton->setDisabled(true);
    mOkButton->setDefault(true);
}

void ConditionDialog::setEditingCondition(AbstractCondition * condition)
{
    if (!mCondition)
        return;

    if (!condition) {
        reset();
        return;
    }

    SimpleCondition* scondition = dynamic_cast<SimpleCondition*>(condition);
    if (!scondition)
        return;

    mEditingCondition = scondition;

    int index = mCondition->indexOfToken(condition);
    if (index == 0)
        mLogicalOperators->setVisible(false);
    else if (index > 0) {
        mLogicalOperators->setVisible(true);
        ConditionToken* token = mCondition->tokenAt(index-1);
        if (token && token->type() == ConditionTokenMetaType::LogicalOperator) {
            setCurrentLogicalOperator(token->value().toInt());
        }
    }

    ConditionToken leftToken = scondition->leftOperand();
    ConditionOperation::Type op = scondition->operation();
    ConditionToken rightToken = scondition->rightOperand();

    mLeftMemberEdit->setText(leftToken.value().toString());
    setDataType(mDataType1Chooser, leftToken.type());
    setCurrentOperation(op);
    mRightMemberEdit->setText(rightToken.value().toString());
    setDataType(mDataType2Chooser, rightToken.type());

    setEditMode(true);
}

AbstractCondition* ConditionDialog::editingCondition() const
{
    return mEditingCondition;
}

ConditionToken* ConditionDialog::editingLogicalOperator() const
{
    if (!mCondition)
        return 0;

    int index = mCondition->indexOfToken(mEditingCondition);
    return mCondition->tokenAt(index-1);
}

void ConditionDialog::setEditMode(bool edit)
{
    if (mEditMode == edit)
        return;

    if (edit) {
        mLeftMemberEdit->setStyleSheet(QString("background-color: %1").arg(EDIT_BG_COLOR));
        mRightMemberEdit->setStyleSheet(QString("background-color: %1").arg(EDIT_BG_COLOR));
        QIcon icon = QApplication::style()->standardIcon(QStyle::SP_DialogApplyButton);
        mAddButton->setIcon(icon);
        mAddButton->setEnabled(true);
    }
    else {
        mLeftMemberEdit->setStyleSheet("");
        mRightMemberEdit->setStyleSheet("");
        mAddButton->setIcon(mAddIcon);
    }
}

bool ConditionDialog::editMode() const
{
    return mEditMode;
}

void ConditionDialog::showEvent(QShowEvent * event)
{
    reset();
    QDialog::showEvent(event);
}
