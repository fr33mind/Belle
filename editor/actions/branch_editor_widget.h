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

class Branch;

class BranchEditorWidget : public ActionEditorWidget
{
    Q_OBJECT
public:
    explicit BranchEditorWidget(QWidget *parent = 0);
    virtual void updateData(Action*);
    bool eventFilter(QObject *, QEvent *);

protected:

signals:
    
private slots:
    void onConditionsClicked();
    void onAddItemActivated();
    void onConditionChanged();
    void onItemActivated(int);
    void onItemRemoved(int);

private:
    QTextEdit* mConditionEdit;
    ComboBox* mTrueActionsChooser;
    ComboBox* mFalseActionsChooser;
    
};

#endif // BRANCH_EDITOR_WIDGET_H
