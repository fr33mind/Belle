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

#ifndef OBJECTGROUP_EDITOR_WIDGET_H
#define OBJECTGROUP_EDITOR_WIDGET_H

#include "object_editor_widget.h"

#include "objectgroup.h"

class ObjectGroup;

class ObjectGroupEditorWidget : public ObjectEditorWidget
{
    Q_OBJECT
public:
    explicit ObjectGroupEditorWidget(QWidget *parent = 0);

signals:

public slots:

private:

};

#endif // OBJECTGROUP_EDITOR_WIDGET_H
