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

#include "gotolabel_editor_widget.h"

#include <QLineEdit>
#include <QDebug>

#include "scene.h"

GoToLabelEditorWidget::GoToLabelEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mCurrentGoToLabel = 0;

    mLabelEdit = new QLineEdit(this);
    connect(mLabelEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onLabelEdited(const QString&)));

    beginGroup(tr("Go to Label Editor"));
    appendRow(tr("Label"), mLabelEdit);
    endGroup();
}

void GoToLabelEditorWidget::updateData(Action * action)
{
    mCurrentGoToLabel = qobject_cast<GoToLabel*>(action);

    if (! mCurrentGoToLabel)
        return;

    mLabelEdit->setText(mCurrentGoToLabel->targetLabelName());
    if (mCurrentGoToLabel->hasValidLabel())
        mLabelEdit->setStyleSheet("background-color: rgba(0, 255, 0, 100);");
    else
        mLabelEdit->setStyleSheet("background-color: rgba(255, 0, 0, 100);");
}

void GoToLabelEditorWidget::onLabelEdited(const QString & text)
{
    if (! mCurrentGoToLabel)
        return;

    mCurrentGoToLabel->setTargetLabel(text);

    if (mCurrentGoToLabel->hasValidLabel())
        mLabelEdit->setStyleSheet("background-color: rgba(0, 255, 0, 100);");
    else
        mLabelEdit->setStyleSheet("background-color: rgba(255, 0, 0, 100);");
}
