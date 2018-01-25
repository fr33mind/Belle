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

#ifndef SET_GAME_VARIABLE_EDITOR_WIDGET_H
#define SET_GAME_VARIABLE_EDITOR_WIDGET_H

#include <QComboBox>

#include "action_editor_widget.h"
#include "set_game_variable.h"
#include "variablevalidator.h"

class SetGameVariable;

class SetGameVariableEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

    QLineEdit* mVariableEdit;
    QComboBox* mOperatorChooser;
    QComboBox* mValueTypeChooser;
    QLineEdit* mValueEdit;
    VariableValidator* mVariableValidator;

public:
    explicit SetGameVariableEditorWidget(ActionEditorWidget *parent = 0);
    QStringList operatorsText();
    
signals:
    
protected:
    virtual void updateData(GameObject*);

private slots:
    void onVariableEdited(const QString&);
    void onOperatorChanged(int);
    void onValueTypeChanged(int);
    void onValueEdited(const QString&);
};

#endif // SET_GAME_VARIABLE_EDITOR_WIDGET_H
