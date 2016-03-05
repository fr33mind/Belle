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

#include "scene_manager.h"

#include <QDebug>

#include "utils.h"

static QSize mSceneSize;
static Clipboard *mClipboard = 0;

SceneManager::SceneManager(QObject * parent, const QString& name) :
    QObject(parent)
{
    mCurrentSceneIndex = -1;
    connect(this, SIGNAL(currentSceneChanged()), this, SLOT(onCurrentSceneChanged()));
    setObjectName(name);
}

SceneManager::SceneManager(int width, int height, QObject * parent, const QString& name) :
    QObject(parent)
{
    Scene::setWidth(width);
    Scene::setHeight(height);
    mCurrentSceneIndex = -1;
    connect(this, SIGNAL(currentSceneChanged()), this, SLOT(onCurrentSceneChanged()));
    setObjectName(name);
}


SceneManager::~SceneManager()
{
}

void SceneManager::addScene(Scene * scene)
{
    insertScene(size(), scene);
}

void SceneManager::insertScene(int index, Scene * scene)
{
    if (! scene)
        return;

    removeScene(scene);

    connect(scene, SIGNAL(nameChanged(const QString&)), this, SLOT(onSceneNameChanged(const QString&)), Qt::UniqueConnection);
    connect(scene, SIGNAL(dataChanged()), this, SIGNAL(updateDrawingSurfaceWidget()), Qt::UniqueConnection);
    connect(scene, SIGNAL(selectionChanged(Object*)), this, SIGNAL(selectionChanged(Object*)), Qt::UniqueConnection);

    mCurrentSceneIndex = index;
    mGameObjectManager.insert(index, scene);
}

Scene* SceneManager::addScene(const QString& name)
{
    mCurrentSceneIndex = size();
    Scene* scene = new Scene(this, name);
    addScene(scene);
    return scene;
}

void SceneManager::removeScene(Scene* scene, bool del)
{
    removeSceneAt(indexOf(scene), del);
}

void SceneManager::removeSceneAt(int i, bool del)
{
    if (i < 0 || i >= size())
        return;

    Scene* scene = takeAt(i);
    if (scene && del) {
        scene->deleteLater();
    }

    if (size() == 0)
        mCurrentSceneIndex = -1;
}

void SceneManager::removeScenes(bool del)
{
    mGameObjectManager.clear(del);
    mCurrentSceneIndex = -1;
}

int SceneManager::currentSceneIndex()
{
    return mCurrentSceneIndex;
}

void SceneManager::setCurrentSceneIndex(int index)
{
    if (index >= size() || index < 0 || index == mCurrentSceneIndex)
        return;

    mCurrentSceneIndex = index;
    emit currentSceneChanged();
}

void SceneManager::setCurrentScene(Scene* scene)
{
    if (! scene)
        return;

    int index = indexOf(scene);
    if (index != -1)
        setCurrentSceneIndex(index);
}

Scene* SceneManager::currentScene()
{
    Scene* currScene = sceneAt(mCurrentSceneIndex);
    if (currScene)
        return currScene;
    return 0;
}

int SceneManager::size()
{
    return mGameObjectManager.size();
}

int SceneManager::count()
{
    return mGameObjectManager.size();
}

Scene* SceneManager::sceneAt(int i)
{
    return qobject_cast<Scene*>(mGameObjectManager.objectAt(i));
}

Scene* SceneManager::takeAt(int i)
{
    Scene* scene = qobject_cast<Scene*>(mGameObjectManager.takeAt(i));
    if (scene)
        scene->disconnect(this);
    emit sceneRemoved(i);
    return scene;
}

void SceneManager::setClipboard(Clipboard* clipboard)
{
    mClipboard = clipboard;
}

Clipboard* SceneManager::clipboard()
{
    return mClipboard;
}

void SceneManager::onCurrentSceneChanged()
{
    if (currentScene())
        currentScene()->selectObject(currentScene()->selectedObject());
}

int SceneManager::indexOf(Scene* scene)
{
    return mGameObjectManager.indexOf(scene);
}

bool SceneManager::contains(const QString& name)
{
    return mGameObjectManager.contains(name);
}

bool SceneManager::contains(Scene* scene) const
{
    return mGameObjectManager.contains(scene);
}

QList<Scene*> SceneManager::scenes()
{
    QList<GameObject*> objects = mGameObjectManager.objects();
    QList<Scene*> scenes;
    foreach(GameObject* obj, objects) {
        scenes << qobject_cast<Scene*>(obj);
    }

    return scenes;
}

void SceneManager::resizeScenes(int w, int h, bool pos, bool size)
{
    QList<Scene*> scenes = this->scenes();
    for(int i=0; i < scenes.size(); i++)
        scenes.at(i)->resize(w, h, pos, size);
}

void SceneManager::onSceneNameChanged(const QString & name)
{
    Scene* scene = qobject_cast<Scene*>(sender());
    if (scene)
        emit sceneNameChanged(indexOf(scene), name);
}
