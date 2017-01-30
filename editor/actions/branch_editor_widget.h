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

#ifndef BRANCH_EDITOR_WIDGET_H
#define BRANCH_EDITOR_WIDGET_H

#include <QTextEdit>

#include "action_editor_widget.h"
#include "branch.h"
#include "condition_text_edit.h"
#include "actionmanagerbutton.h"

class Branch;
class ConditionTextEdit;

class BranchEditorWidget : public ActionEditorWidget
{
    Q_OBJECT
public:
    explicit BranchEditorWidget(QWidget *parent = 0);
    bool eventFilter(QObject *, QEvent *);

protected:
    virtual void updateData(GameObject*);
    virtual void disconnectGameObject();

signals:
    
private slots:
    void onConditionsClicked();

private:
    ConditionTextEdit* mConditionEdit;
    ActionManagerButton* mTrueActionsButton;
    ActionManagerButton* mFalseActionsButton;
    
};

#endif // BRANCH_EDITOR_WIDGET_H
