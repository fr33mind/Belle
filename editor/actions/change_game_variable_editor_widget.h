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

#ifndef GAME_VARIABLE_EDITOR_WIDGET_H
#define GAME_VARIABLE_EDITOR_WIDGET_H

#include <QComboBox>

#include "action_editor_widget.h"
#include "change_game_variable.h"

class ChangeGameVariable;

class ChangeGameVariableEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

    QLineEdit* mVariableEdit;
    QComboBox* mOperatorChooser;
    QLineEdit* mValueEdit;

public:
    explicit ChangeGameVariableEditorWidget(ActionEditorWidget *parent = 0);
    QStringList operatorsText();
    void updateData(Action *);
    
signals:
    
private slots:
    void onVariableEdited(const QString&);
    void onOperatorChanged(int);
    void onValueEdited(const QString&);
};

#endif // SET_GAME_VARIABLE_EDITOR_WIDGET_H
