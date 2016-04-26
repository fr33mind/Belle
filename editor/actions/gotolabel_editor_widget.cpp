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

#include "scene.h"

GoToLabelEditorWidget::GoToLabelEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mLabelChooser = new QComboBox(this);
    mLabelChooser->setEditable(true);
    connect(mLabelChooser, SIGNAL(currentTextChanged(const QString&)), this, SLOT(onLabelChanged(const QString&)));

    beginGroup(tr("Go to Label"));
    appendRow(tr("Label"), mLabelChooser);
    endGroup();
}

void GoToLabelEditorWidget::updateData(GameObject* action)
{
    ActionEditorWidget::updateData(action);
    GoToLabel* gotolabel = qobject_cast<GoToLabel*>(action);
    if (! gotolabel)
        return;

    loadLabels(gotolabel);
    checkLabelValidity(gotolabel);
}

void GoToLabelEditorWidget::onLabelChanged(const QString & text)
{
    GoToLabel* gotolabel = qobject_cast<GoToLabel*>(mGameObject);
    if (! gotolabel)
        return;

    gotolabel->setTargetLabel(text);
    checkLabelValidity(gotolabel);
}

void GoToLabelEditorWidget::checkLabelValidity(GoToLabel *gotolabel)
{
    if (gotolabel->hasValidLabel())
        mLabelChooser->lineEdit()->setStyleSheet("background-color: rgba(0, 255, 0, 100);");
    else
        mLabelChooser->lineEdit()->setStyleSheet("background-color: rgba(255, 0, 0, 100);");
}

void GoToLabelEditorWidget::loadLabels(GoToLabel* gotolabel)
{
    mLabelChooser->clear();

    QList<Label*> labels = gotolabel->availableLabels();
    for(int i=0; i < labels.size(); i++) {
        mLabelChooser->addItem(labels.at(i)->name());
    }

    int index = mLabelChooser->findText(gotolabel->targetLabelName());
    mLabelChooser->setCurrentIndex(index);
    if (index == -1)
        mLabelChooser->setCurrentText(gotolabel->targetLabelName());
}
