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

#ifndef GET_USER_INPUT_EDITOR_WIDGET_H
#define GET_USER_INPUT_EDITOR_WIDGET_H

#include "action_editor_widget.h"

class GetUserInput;

class GetUserInputEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

public:
    explicit GetUserInputEditorWidget(QWidget *parent = 0);

signals:

protected:
    virtual void updateData(GameObject*);
    
private slots:
    void onMessageChanged(const QString&);
    void onVariableChanged(const QString&);
    void onDefaultValueChanged(const QString&);

private:
    QLineEdit* mMessageEdit;
    QLineEdit* mVariableEdit;
    QLineEdit* mDefaultValueEdit;
};

#endif // GET_USER_INPUT_EDITOR_WIDGET_H
