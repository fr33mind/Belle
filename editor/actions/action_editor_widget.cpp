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

#include "action_editor_widget.h"

ActionEditorWidget::ActionEditorWidget(QWidget *parent) :
    PropertiesWidget(parent)
{
    mAction = 0;
    mNameEdit = new QLineEdit(this);
    connect(mNameEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onNameEdited(const QString&)));
    beginGroup(tr("Action"), "Action");
    appendRow("Name", mNameEdit);
    endGroup();

}

void ActionEditorWidget::updateData(Action * action)
{
    if (action == mAction)
        return;

    if (mAction)
        mAction->disconnect(this);

    if (action)
        connect(action, SIGNAL(destroyed()), this, SLOT(actionDestroyed()));

    mAction = action;
    if (! mAction)
        return;

    mNameEdit->setText(mAction->objectName());

}

void ActionEditorWidget::actionDestroyed()
{
    mAction = 0;
}

Action* ActionEditorWidget::action()
{
    return 0;
}

void ActionEditorWidget::setAction(Action* action)
{
    mAction = action;
}

void ActionEditorWidget::onNameEdited(const QString& name)
{
    if (mAction)
        mAction->setObjectName(name);
}

void ActionEditorWidget::setObjects(QList<Object *> & objs)
{
    mObjects = objs;
}

QList<Object *> ActionEditorWidget::objects()
{
    return mObjects;
}

ActionEditorWidget* ActionEditorWidget::copy()
{
    ActionEditorWidget* awidget = new ActionEditorWidget();
    awidget->append(this);
    return awidget;
}
