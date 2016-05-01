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

#ifndef CONDITION_DIALOG_H
#define CONDITION_DIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QRadioButton>

class ConditionWidget;

class ConditionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConditionDialog(const QString&, QWidget *parent = 0);
    QString condition();
private:
    QString determineType(const QString&);
    
signals:
    
private slots:
    void onAddClicked();
    void onCurrentOperatorChanged(int);
    void onVariableEdited(const QString&);
    void onValueEdited(const QString&);
    void onTypeChanged(int);

private:
    ConditionWidget* mConditionWidget;
    QLineEdit* mEditVariable;
    QComboBox* mOperatorsComboBox;
    QLineEdit* mEditValue;
    QComboBox* mComboValueType;
    QRadioButton * mAndRadioButton;
    QRadioButton * mOrRadioButton;
    QComboBox * mLogicalOperators;
    QPushButton* mAddButton;
    QComboBox* mDataType1Chooser;
    QComboBox* mDataType2Chooser;
    QValidator* mVariableValidator;
};

#endif // Condition_DIALOG_H
