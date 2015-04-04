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

#include "gotoscene_editorwidget.h"

#include "scene.h"
#include "scene_manager.h"

GoToSceneEditorWidget::GoToSceneEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mSceneEdit = new QComboBox(this);
    mSceneEdit->addItem(tr("Next"), QVariant(GoToScene::Position));
    mSceneEdit->addItem(tr("Previous"), QVariant(GoToScene::Position));
    mSceneEdit->addItem(tr("First"), QVariant(GoToScene::Position));
    mSceneEdit->addItem(tr("Last"), QVariant(GoToScene::Position));
    mSceneEdit->insertSeparator(5);

    connect(mSceneEdit, SIGNAL(currentIndexChanged(int)), this, SLOT(onSceneChanged(int)));

    beginGroup(tr("Go to scene Editor"));
    appendRow(tr("Scene"), mSceneEdit);
    endGroup();

    resizeColumnToContents(0);
}

void GoToSceneEditorWidget::updateData(Action * action)
{
    GoToScene* goToScene = qobject_cast<GoToScene*>(action);
    if (! goToScene)
        return;

    ActionEditorWidget::updateData(action);
    mAction = 0;

    //remove all scenes for update
    for(int i=mSceneEdit->count()-1; i > 4; i--)
        mSceneEdit->removeItem(i);

    Scene * scene = goToScene->scene();
    if (scene && scene->sceneManager()) {
        QList<Scene*> scenes = scene->sceneManager()->scenes();
        for(int i=0; i < scenes.size(); i++) {
            mSceneEdit->addItem(scenes[i]->objectName(), QVariant(GoToScene::Name));
        }
    }

    QString currScene = goToScene->targetScene();
    int i=0;
    for(; i < mSceneEdit->count(); i++) {
        if (mSceneEdit->itemText(i) == currScene)
            break;
    }

    if (i >= 0 && i < mSceneEdit->count())
        mSceneEdit->setCurrentIndex(i);

    mAction = action;
}

void GoToSceneEditorWidget::onSceneChanged(int index)
{
    GoToScene* goToScene = qobject_cast<GoToScene*>(mAction);
    if (! goToScene)
        return;

    QString name = mSceneEdit->itemText(index);
    QVariant data = mSceneEdit->itemData(index);

    goToScene->setTargetScene(name, static_cast<GoToScene::TargetType>(data.toInt()));
}
