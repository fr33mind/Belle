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

#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <QList>
#include <QResizeEvent>
#include <QObject>

#include "scene.h"
#include "clipboard.h"

class Scene;
class Clipboard;

class SceneManager : public QObject
{
    Q_OBJECT

    GameObjectManager mGameObjectManager;
    int mCurrentSceneIndex;
    
    public:
        explicit SceneManager(QObject * parent=0, const QString& name="SceneManager");
        virtual ~SceneManager();
        Scene* addScene(const QString& name="");
        void addScene(Scene*) ;
        Scene* scene(const QString&) const;
        Scene* sceneAt(int);
        Scene* takeAt(int);
        void removeScenes(bool del);
        void setCurrentScene(Scene*);
        void setCurrentSceneIndex(int);
        void removeScene(Scene*, bool del=false);
        void removeSceneAt(int, bool del=false);
        void insertScene(int, Scene*);
        QList<Scene*> scenes();
        int count();
        int size();
        int currentSceneIndex();
        Scene * currentScene();
        bool contains(const QString& name);
        bool contains(Scene*) const;
        int indexOf(Scene*);
        void resizeScenes(int, int, bool, bool);

        static void setClipboard(Clipboard*);
        static Clipboard* clipboard();
        
    signals:
        void resized(const QResizeEvent&);
        void updateDrawingSurfaceWidget();
        void currentSceneChanged();
        void selectionChanged(Object*);
        void sceneRemoved(int);
        void sceneNameChanged(int, const QString&);

    private slots:
        void onCurrentSceneChanged();
        void onSceneNameChanged(const QString&);

};


#endif
