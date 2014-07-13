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

#include "lineedit.h"
#include "condition_widget.h"
#include "utils.h"

ConditionDialog::ConditionDialog(const QString& condition, QWidget *parent) :
    QDialog(parent)
{
    mVariableValidator = new QRegExpValidator(QRegExp("[a-zA-Z]+[0-9]*"), this);
    QVBoxLayout* vlayout1 = 0, * vlayout2 = 0;
    setWindowTitle(tr("Edit Condition"));
    QVBoxLayout* mainLayout = new QVBoxLayout;
    if (this->layout())
        this->layout()->deleteLater();

    setLayout(mainLayout);

    mConditionWidget = new ConditionWidget(condition, this);

    //Create logical operator ComboBox
    mLogicalOperators = new QComboBox(this);
    mLogicalOperators->addItem(tr("And"));
    mLogicalOperators->addItem(tr("Or"));
    mLogicalOperators->setVisible(false);

    //edit variable widget
    mEditVariable = new LineEdit(tr("Variable"), this);
    mEditVariable->setObjectName("variableEditor");
    //QRegExpValidator *validator = new QRegExpValidator(QRegExp("([a-zA-Z]+[0-9]*)*|('|\")[^ '\"]*('|\")|[0-9]+\.[0-9]*"), this);

    mEditVariable->setValidator(mVariableValidator);
    connect(mEditVariable, SIGNAL(textEdited(const QString&)), this, SLOT(onVariableEdited(const QString&)));

    //data type combobox
    mDataType1Chooser = new QComboBox(this);
    mDataType1Chooser->setObjectName("type1");
    mDataType1Chooser->addItem(tr("Auto"), "auto");
    mDataType1Chooser->addItem(tr("Variable"), "variable");
    mDataType1Chooser->addItem(tr("Value"), "value");
    connect(mDataType1Chooser, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypeChanged(int)));

    mOperatorsComboBox = new QComboBox(this);
    QStringList operators = mConditionWidget->operatorsText();
    foreach(const QString& op, operators) {
        mOperatorsComboBox->addItem(op);
    }
    connect(mOperatorsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentOperatorChanged(int)));

    mEditValue = new LineEdit(tr("Value"),this);
    mEditVariable->setObjectName("valueEditor");
    mEditValue->setToolTip(tr("Values are interpreted as a string, if you want to compare to a variable instead, use the dollar sign before name (eg. $objects)"));
    connect(mEditValue, SIGNAL(textEdited(const QString&)), this, SLOT(onValueEdited(const QString&)));

    //data type combobox
    mDataType2Chooser = new QComboBox(this);
    mDataType2Chooser->setObjectName("type2");
    for(int i=0; i < mDataType1Chooser->count(); i++)
        mDataType2Chooser->addItem(mDataType1Chooser->itemText(i), mDataType1Chooser->itemData(i));
    connect(mDataType2Chooser, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypeChanged(int)));

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    //create add button
    mAddButton = new QPushButton(QIcon(":/media/add.png"), "", this);
    mAddButton->setEnabled(false);
    connect(mAddButton, SIGNAL(clicked()), this, SLOT(onAddClicked()));

    //create first vertical layout
    vlayout1 = new QVBoxLayout();
    vlayout1->addWidget(mEditVariable);
    vlayout1->addWidget(mDataType1Chooser);

    //create second vertical layout
    vlayout2 = new QVBoxLayout();
    vlayout2->addWidget(mEditValue);
    vlayout2->addWidget(mDataType2Chooser);

    //add all widgets and layouts to parent horizontal layout
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(mLogicalOperators);
    hlayout->addLayout(vlayout1);
    hlayout->addWidget(mOperatorsComboBox);
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
    if (! model)
        return;

    if (mLogicalOperators->isVisible())
        mConditionWidget->appendLogicalOperator(mLogicalOperators->currentText(), mLogicalOperators->currentIndex());

    if (mEditValue->isVisible()) {
        mConditionWidget->appendCondition(mEditVariable->text(), mOperatorsComboBox->currentIndex(), mEditValue->text());
    }
    else {
        // zero index means "=="
        mConditionWidget->appendCondition(mEditVariable->text(), 0, mOperatorsComboBox->currentText());
    }

    if (mLogicalOperators->isHidden())
        mLogicalOperators->setVisible(true);

}

void ConditionDialog::onCurrentOperatorChanged(int index)
{
    if (index >= mOperatorsComboBox->count()-4){
        mEditValue->hide();
        mDataType2Chooser->hide();
        mDataType1Chooser->setCurrentIndex(1); //set "variable"
        mDataType1Chooser->setEnabled(false);
    }
    else if (index == 2) { //"in" operator - invert variable and value position
        mDataType1Chooser->setCurrentIndex(2); //set "value"
        mDataType2Chooser->setCurrentIndex(1); //set "variable
        mDataType1Chooser->setEnabled(false);
        mDataType2Chooser->setEnabled(false);
    }
    else {
        mEditValue->show();
        mDataType2Chooser->show();
        if (! mDataType1Chooser->isEnabled())
            mDataType1Chooser->setEnabled(true);
        if (! mDataType2Chooser->isEnabled())
            mDataType2Chooser->setEnabled(true);
    }

    onVariableEdited(mEditVariable->text());
}

void ConditionDialog::onVariableEdited(const QString & text)
{
    if (text.isEmpty())
        mAddButton->setEnabled(false);
    else if (! mEditValue->isVisible() || (mEditValue->isVisible() && ! mEditValue->text().isEmpty()))
        mAddButton->setEnabled(true);
    else
        mAddButton->setEnabled(false);
}

void ConditionDialog::onValueEdited(const QString & text)
{
    if (mEditValue->isVisible()) {
        if (text.isEmpty() || mEditVariable->text().isEmpty())
            mAddButton->setEnabled(false);
        else
            mAddButton->setEnabled(true);

    }
    else if (! mEditVariable->text().isEmpty()){
         mAddButton->setEnabled(true);
    }
    else
         mAddButton->setEnabled(false);
}

QString ConditionDialog::condition()
{
    return mConditionWidget->condition();
}

void ConditionDialog::onTypeChanged(int index)
{
    if (index < 0)
        return;

    QString objName = sender()->objectName();
    QComboBox* senderComboBox = static_cast<QComboBox*>(sender());
    QString type = senderComboBox->itemData(index).toString();
    LineEdit* lineEdit = 0;

    if (objName == "type1")
        lineEdit = mEditVariable;
    else
        lineEdit = mEditValue;

    lineEdit->setPlaceholderText(senderComboBox->itemText(index));

    if (type == "auto") {
        type = determineType(lineEdit->text());
    }

    if (type == "variable")
        lineEdit->setValidator(mVariableValidator);
    else
        lineEdit->setValidator(0);
}

QString ConditionDialog::determineType(const QString& value)
{
    if (value.contains(QRegExp("[^a-zA-Z0-9]")) || value.isEmpty() || value.isNull() || value[0].isDigit())
        return "value";

    return "variable";
}
