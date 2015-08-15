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

#ifndef SCENE_EDITOR_WIDGET_H
#define SCENE_EDITOR_WIDGET_H

#include "gameobject_editorwidget.h"
#include "choosefilebutton.h"
#include "color_pushbutton.h"
#include "scene.h"

class Scene;

class SceneEditorWidget : public GameObjectEditorWidget
{
    Q_OBJECT

    ChooseFileButton* mChooseBackgroundButton;
    ColorPushButton* mChooseBackgroundColorButton;

public:
    explicit SceneEditorWidget(QWidget *parent = 0);
    
signals:

protected:
    virtual void updateData(GameObject*);

private slots:
    void onBackgroundSelected(const QString&);
    void onBackgroundColorSelected(const QColor&);
    
};

#endif // SCENE_EDITOR_WIDGET_H
