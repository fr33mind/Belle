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

#include "scene_editor_widget.h"

SceneEditorWidget::SceneEditorWidget(QWidget *parent) :
    GameObjectEditorWidget(parent)
{
    mChooseBackgroundButton = new ChooseFileButton(ChooseFileButton::ImageFilter, this);
    connect(mChooseBackgroundButton, SIGNAL(fileSelected(const QString&)), this, SLOT(onBackgroundSelected(const QString&)));
    mChooseBackgroundColorButton = new ColorPushButton(this);
    connect(mChooseBackgroundColorButton, SIGNAL(colorChosen(QColor)), this, SLOT(onBackgroundColorSelected(const QColor&)));

    beginGroup(tr("Scene Editor"));
    appendRow(tr("Background Image"), mChooseBackgroundButton);
    appendRow(tr("Background Color"), mChooseBackgroundColorButton);
    endGroup();

    resizeColumnToContents(0);
}

void SceneEditorWidget::updateData(GameObject* obj)
{
    GameObjectEditorWidget::updateData(obj);
    Scene* scene = qobject_cast<Scene*>(obj);
    if (scene) {
        mChooseBackgroundButton->setFilePath(scene->backgroundPath());
        mChooseBackgroundColorButton->setColor(scene->backgroundColor());
    }
}

void SceneEditorWidget::onBackgroundSelected(const QString & path)
{
    Scene* scene = qobject_cast<Scene*>(mGameObject);
    if (scene)
        scene->setBackgroundImage(path);
}

void SceneEditorWidget::onBackgroundColorSelected(const QColor & color)
{
    Scene* scene = qobject_cast<Scene*>(mGameObject);
    if (scene)
        scene->setBackgroundColor(color);
}
