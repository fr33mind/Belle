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
}

void ObjectGroupEditorWidget::updateData(Object* obj)
{
    ObjectEditorWidget::updateData(obj);

    /*ObjectGroup * prevObjectGroup = mCurrentObjectGroup;
    mCurrentObjectGroup = qobject_cast<ObjectGroup*>(obj);
    if (! mCurrentObjectGroup)
        return;

    if (mCurrentObjectGroup != prevObjectGroup) {
        this->clear(1);
        QList<Object*> objects = mCurrentObjectGroup->objects();
        for(int i=0; i < objects.size(); i++) {
            this->append(objects[i]->editorWidget());
        }
    }*/
}
