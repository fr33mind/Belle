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
    mSceneComboBox = new GameObjectComboBox(this);
    mSceneComboBox->addItem(tr("Next"), QVariant(GoToScene::Next));
    mSceneComboBox->addItem(tr("Previous"), QVariant(GoToScene::Previous));
    mSceneComboBox->addItem(tr("First"), QVariant(GoToScene::First));
    mSceneComboBox->addItem(tr("Last"), QVariant(GoToScene::Last));
    mSceneComboBox->insertSeparator(5);

    connect(mSceneComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSceneChanged(int)));

    beginGroup(tr("Go to scene Editor"));
    appendRow(tr("Scene"), mSceneComboBox);
    endGroup();

    resizeColumnToContents(0);
}

void GoToSceneEditorWidget::updateData(GameObject* action)
{
    ActionEditorWidget::updateData(action);

    GoToScene* goToScene = qobject_cast<GoToScene*>(action);
    if (! goToScene)
        return;

    //remove all scenes for update
    for(int i=mSceneComboBox->count()-1; i > 4; i--)
        mSceneComboBox->removeItem(i);

    if (!goToScene->isSynced() || !goToScene->resource()) {
        Scene * scene = goToScene->scene();
        if (scene && scene->sceneManager()) {
            QList<Scene*> scenes = scene->sceneManager()->scenes();
            foreach(Scene* scene, scenes) {
                mSceneComboBox->addObject(scene);
            }
        }
    }

    //Remove separator if there are no scenes
    if (mSceneComboBox->count() == 5)
        mSceneComboBox->removeItem(4);

    Scene* targetScene = goToScene->targetScene();
    if (targetScene) {
        mSceneComboBox->setCurrentObject(targetScene);
        if (mSceneComboBox->currentObject() != targetScene)
            mSceneComboBox->setCurrentIndex(-1);
    }
    else {
        int mtarget = static_cast<int>(goToScene->metaTarget());
        int index = mSceneComboBox->findData(mtarget);
        mSceneComboBox->setCurrentIndex(index);
    }
}

void GoToSceneEditorWidget::onSceneChanged(int index)
{
    GoToScene* goToScene = qobject_cast<GoToScene*>(mGameObject);
    if (! goToScene)
        return;

    QVariant data = mSceneComboBox->itemData(index);

    if (index < 4)
        goToScene->setMetaTarget(static_cast<GoToScene::MetaTarget>(data.toInt()));
    else
        goToScene->setTargetScene(data.value<Scene*>());
}
