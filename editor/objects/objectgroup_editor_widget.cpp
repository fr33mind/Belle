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

#include "objectgroup_editor_widget.h"

#include <QDebug>

ObjectGroupEditorWidget::ObjectGroupEditorWidget(QWidget *parent) :
    ObjectEditorWidget(parent)
{
    mAlignCheckBox = new QCheckBox(this);
    mResizeToContentsCheckBox = new QCheckBox(this);
    mObjectsSyncedCheckBox = new QCheckBox(this);

    beginGroup(tr("Object Group"), "ObjectGroup");
    appendRow(tr("Align"), mAlignCheckBox);
    appendRow(tr("Resize to Contents"), mResizeToContentsCheckBox);
    appendRow(tr("Sync Objects"), mObjectsSyncedCheckBox);
    endGroup();

    connect(mAlignCheckBox, SIGNAL(toggled(bool)), this, SLOT(onAlignToggled(bool)));
    connect(mResizeToContentsCheckBox, SIGNAL(toggled(bool)), this, SLOT(onResizeToContentsToggled(bool)));
    connect(mObjectsSyncedCheckBox, SIGNAL(toggled(bool)), this, SLOT(onObjectsSyncedToggled(bool)));

    resizeColumnToContents(0);
}

void ObjectGroupEditorWidget::updateData(GameObject* object)
{
    ObjectEditorWidget::updateData(object);
    ObjectGroup* obj = qobject_cast<ObjectGroup*>(object);
    if (! obj)
        return;

    mAlignCheckBox->setChecked(obj->isAlignEnabled());
    mResizeToContentsCheckBox->setChecked(obj->resizeToContentsEnabled());
    mObjectsSyncedCheckBox->setChecked(obj->objectsSynced());
}

void ObjectGroupEditorWidget::onAlignToggled(bool checked)
{
    ObjectGroup* obj = qobject_cast<ObjectGroup*>(mGameObject);
    if (! obj)
        return;

    obj->setAlignEnabled(checked);
}

void ObjectGroupEditorWidget::onResizeToContentsToggled(bool checked)
{
    ObjectGroup* obj = qobject_cast<ObjectGroup*>(mGameObject);
    if (! obj)
        return;

    obj->setResizeToContentsEnabled(checked);
}

void ObjectGroupEditorWidget::onObjectsSyncedToggled(bool checked)
{
    ObjectGroup* obj = qobject_cast<ObjectGroup*>(mGameObject);
    if (! obj)
        return;

    obj->setObjectsSynced(checked);
}
