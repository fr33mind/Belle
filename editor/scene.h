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

#ifndef SCENE_H
#define SCENE_H

#include <QList>
#include <QSize>
#include <QObject>
#include <QWidget>
#include <QResizeEvent>
#include <QSize>

#include "imagefile.h"
#include "gameobject.h"
#include "gameobjectmanager.h"

class SceneManager;
class Object;
class Action;

class Scene : public GameObject
{
    Q_OBJECT
    
    GameObjectManager mObjectManager;
    GameObjectManager mTemporaryObjectManager;
    GameObjectManager* mActionManager;
    Object * mSelectedObject;
    Object* mHighlightedObject;
    ImageFile *mBackgroundImage;
    ImageFile *mTemporaryBackgroundImage;
    QColor mBackgroundColor;
    QColor mTemporaryBackgroundColor;
    
    public:
        explicit Scene(QObject *parent = 0, const QString& name="");
        Scene(const QVariantMap& data, QObject *parent = 0);
        ~Scene();
        SceneManager* sceneManager();
        QList<Object*> objects() const;
        QList<Object*> objects(GameObjectMetaType::Type) const;
        QList<Object*> temporaryObjects() const;
        Object* objectAt (qreal, qreal);
        Object* object(const QString&);
        void appendObject(Object*, bool select=true, bool temporarily=false);
        void addCopyOfObject(Object*, bool select=true);

        static int width();
        static int height();
        static QSize size();
        static void setWidth(int);
        static void setHeight(int);
        static void setSize(const QSize&);
        static QPoint point();
        static void setPoint(const QPoint&);

        void setTemporaryBackgroundImage(ImageFile*);
        ImageFile* temporaryBackgroundImage();

        void setTemporaryBackgroundColor(const QColor&);
        QColor temporaryBackgroundColor();

        void selectObjectAt(int, int);
        void moveSelectedObject(int, int);
        void stopMoveSelectedObject();
        Object *selectedObject();
        void setBackgroundImage(const QString&);
        ImageFile* backgroundImage();
        void setBackgroundColor(const QColor&);
        QColor backgroundColor();
        QString backgroundPath();
        ImageFile* background() const;
        void removeSelectedObject(bool del=false);
        bool removeObject(Object*, bool del=false);
        void selectObject(Object*);
        void highlightObject(Object*);
        Object * highlightedObject();
        Scene * copy();
        void resize(int, int, bool, bool);

        void insertAction(int, Action*, bool copy=false);
        void removeActionAt(int, bool del=false);
        void removeAction(Action*, bool del=false);
        QList<Action*> actions() const;
        void appendAction(Action*, bool copy=false);
        Action* actionAt(int) const;
        GameObjectManager* actionManager() const;

        int indexOf(GameObject*);

        virtual QVariantMap toJsonObject(bool internal=true);
        QIcon icon();
        QPixmap* pixmap();

        void show();
        void hide();

        void paint(QPainter&);
        
    protected:
        void _appendObject(Object*, bool temporary=false);
        void _reorderObject(Object*);

    private slots:
        void onResizeEvent(const QResizeEvent&);
        void clearRemovedObject(Object*);
        void onSelectedObjectDestroyed();
        void onHighlightedObjectDestroyed();

    public slots:
        void moveSelectedObjectUp();
        void moveSelectedObjectDown();
        void fillWidth();
        void clearBackground();
        
    signals:
       void resized(const QResizeEvent&);
       void selectionChanged(Object*);
       void actionInserted(int, Action*);
       void actionRemoved(int);
       void actionRemoved(Action*);
       void objectAdded(Object*);
       void objectRemoved(Object*);
       void loaded();

private:
       void init(const QString&);
       void removeTemporaryBackground();
};


#endif
