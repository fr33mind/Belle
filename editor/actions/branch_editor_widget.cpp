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

#include "branch_editor_widget.h"

#include <QComboBox>
#include <QDebug>

#include "combobox.h"
#include "add_action_dialog.h"
#include "condition_dialog.h"

BranchEditorWidget::BranchEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mConditionEdit = new ConditionTextEdit(this);
    mConditionEdit->installEventFilter(this);
    mConditionEdit->setMaximumHeight(50);
    mConditionEdit->setReadOnly(true);

    beginGroup(tr("Condition"));
    appendRow(tr("Contidition"), mConditionEdit);
    endGroup();

    beginGroup(tr("If True"));
    mTrueActionsButton = new ActionManagerButton(this);
    appendRow(tr("Actions"), mTrueActionsButton);
    endGroup();

    beginGroup(tr("If False"));
    mFalseActionsButton = new ActionManagerButton(this);
    appendRow(tr("Actions"), mFalseActionsButton);
    endGroup();

    resizeColumnToContents(0);
}

void BranchEditorWidget::updateData(GameObject* action)
{
    ActionEditorWidget::updateData(action);
    Branch* branch = qobject_cast<Branch*>(action);
    if (! branch)
        return;

    connect(mConditionEdit, SIGNAL(conditionChanged()), branch, SLOT(onConditionChanged()), Qt::UniqueConnection);
    connect(mConditionEdit, SIGNAL(conditionChanged(AbstractCondition*)), branch, SLOT(setCondition(AbstractCondition*)), Qt::UniqueConnection);

    mConditionEdit->clear();
    mConditionEdit->setCondition(branch->condition());

    connect(mTrueActionsButton, SIGNAL(managingFinished()), branch, SLOT(updateDisplayText()), Qt::UniqueConnection);
    connect(mFalseActionsButton, SIGNAL(managingFinished()), branch, SLOT(updateDisplayText()), Qt::UniqueConnection);

    mTrueActionsButton->setActionManager(branch->actionManager(true));
    mFalseActionsButton->setActionManager(branch->actionManager(false));
}

void BranchEditorWidget::onConditionsClicked()
{
}

bool BranchEditorWidget::eventFilter(QObject * obj, QEvent * event)
{
    if (obj == mConditionEdit && event->type() == QEvent::MouseButtonPress)
        onConditionsClicked();

    return ActionEditorWidget::eventFilter(obj, event);
}

void BranchEditorWidget::disconnectGameObject()
{
    ActionEditorWidget::disconnectGameObject();

    if (mGameObject) {
        mConditionEdit->disconnect(mGameObject);
        mTrueActionsButton->disconnect(mGameObject);
        mFalseActionsButton->disconnect(mGameObject);
    }
}
