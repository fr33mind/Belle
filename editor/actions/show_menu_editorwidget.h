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

#ifndef SHOW_MENU_EDITORWIDGET_H
#define SHOW_MENU_EDITORWIDGET_H

#include <QTextEdit>
#include <QComboBox>

#include "action_editor_widget.h"
#include "show_menu.h"
#include "combobox.h"
#include "object_combobox.h"
#include "actionmanagerbutton.h"

#define NUMBER_OF_OPTIONS 8

class ShowMenu;
class Menu;
class ObjectComboBox;
class ActionManagerButton;

class ShowMenuEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

    QList<QLineEdit*> mTextEdits;
    QList<QTextEdit*> mConditionEdits;
    QList<ActionManagerButton*> mActionButtons;
    QComboBox* mChooseNumberOfOptions;
    int mFirstOptionIndex;
    ObjectComboBox* mMenuComboBox;
    QComboBox* mMenuHAlignmentComboBox;
    QComboBox* mMenuVAlignmentComboBox;

public:
    explicit ShowMenuEditorWidget(QWidget *parent = 0);

protected:
    virtual void updateData(GameObject*);

private:
    void setNumberOfOptions(int);
    void _updateTexts(Menu*);

signals:

public slots:
    void onTextEdited(const QString&);
    void onNumberOfOptionsChanged(int);
    void onConditionChanged();

private slots:
    void onMenuChanged(Object*);
    void updateShowMenuText();
    void onMenuHAlignmentChanged(int);
    void onMenuVAlignmentChanged(int);

};

#endif // SHOW_MENU_EDITORWIDGET_H
