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

#ifndef SHOW_EDITOR_WIDGET_H
#define SHOW_EDITOR_WIDGET_H

#include "change_visibility_editor_widget.h"
#include "show.h"

class Show;

class ShowEditorWidget : public ChangeVisibilityEditorWidget
{
    Q_OBJECT

protected:
    QComboBox * mObjectsWidget;
    QComboBox* mObjectStateWidget;

public:
    explicit ShowEditorWidget(QWidget *parent = 0);
    void updateData(Action *);

private slots:
    void onStateChanged(int);
    void onObjectChanged(Object*);

};

#endif // SHOW_EDITOR_WIDGET_H
