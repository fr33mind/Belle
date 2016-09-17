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

#ifndef GOTOSCENE_EDITORWIDGET_H
#define GOTOSCENE_EDITORWIDGET_H

#include "action_editor_widget.h"
#include "gotoscene.h"
#include "gameobjectcombobox.h"

class GoToScene;
class GameObjectComboBox;

class GoToSceneEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

    GameObjectComboBox* mSceneComboBox;

public:
    explicit GoToSceneEditorWidget(QWidget *parent = 0);

signals:

protected:
    virtual void updateData(GameObject*);
public slots:
    void onSceneChanged(int);

};

#endif // GOTOSCENE_EDITORWIDGET_H
