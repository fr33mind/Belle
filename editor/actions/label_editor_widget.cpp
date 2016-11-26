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

#include "label_editor_widget.h"

#include <QLineEdit>
#include <QDebug>

#include "scene.h"

LabelEditorWidget::LabelEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mLabelEdit = new QLineEdit(this);
    connect(mLabelEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onLabelEdited(const QString&)));

    QStandardItem* groupItem = groupAt(0);
    if (groupItem) {
        QModelIndex index = groupItem->index();
        setRowHidden(index.row(), index.parent(), true);
    }

    beginGroup(tr("Label Editor"));
    appendRow(tr("Label"), mLabelEdit);
    endGroup();
}

void LabelEditorWidget::updateData(GameObject* action)
{
    ActionEditorWidget::updateData(action);
    Label* label = qobject_cast<Label*>(action);
    if (! label)
        return;

    mLabelEdit->setText(label->name());
    validateLabelEdit(label);
}

void LabelEditorWidget::onLabelEdited(const QString & text)
{
    Label* label = qobject_cast<Label*>(mGameObject);
    if (! label)
        return;

    bool valid = validateLabelEdit(label);
    if (valid)
        label->setName(text);
}

bool LabelEditorWidget::validateLabelEdit(Label* label)
{
    QString text = mLabelEdit->text();
    Scene* scene = label->scene();
    QList<Action*> actions;
    if (scene)
        actions = scene->actions();

    bool validName = true;
    Label* _label = 0;
    for(int i=0; i < actions.size(); i++) {
        _label = qobject_cast<Label*>(actions[i]);
        if (_label && _label != label && _label->name() == text) {
            validName = false;
            break;
        }
    }

    if (validName) {
        mLabelEdit->setStyleSheet("");
        return true;
    }

    mLabelEdit->setStyleSheet("background-color: rgba(255, 0, 0, 100);");
    return false;
}
