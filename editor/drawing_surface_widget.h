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

#ifndef SCENE_WIDGET_H
#define SCENE_WIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QScrollArea>

#include "scene_manager.h"

#define MARGIN 20

class DrawingSurfaceWidget : public QWidget
{
    Q_OBJECT
    
    SceneManager *mSceneManager;
    bool mMousePressed;
    bool mResizing;
    bool mCanResize;
    bool mMoving;
    bool mCanMove;
    QAction *mMoveUp;
    QAction *mMoveDown;
    QAction *mFillWidth;
    QAction *mClearBackground;
    QAction *mCopyObject;
    QAction *mCutObject;
    QAction *mPasteObject;
    QAction *mDeleteObject;
    QAction *mEditObject;
    QAction *mCancelEditObject;
    QAction *mAlignHorizontally;
    QAction *mAlignVertically;
    Object* mObject;
    
    public:
        explicit DrawingSurfaceWidget(QWidget* parent=0);
        DrawingSurfaceWidget(SceneManager* sceneManager, QWidget* parent=0);
        ~DrawingSurfaceWidget();
        virtual bool eventFilter(QObject *, QEvent *);
        void paintSceneTo(QPaintDevice*);
        void paintObject(QPaintDevice*);
        void setObject(Object*);
        Object* object();
        void setSceneManager(SceneManager*);
        void drawSelection(QPainter&, Object*);
        static QWidget* instance();
        Object* objectAt(qreal, qreal);
        Object* selectedObject();

    protected:
        void paintEvent(QPaintEvent*);
        void mousePressEvent ( QMouseEvent * );
        void mouseReleaseEvent ( QMouseEvent * );
        void mouseMoveEvent ( QMouseEvent * );
        void resizeEvent ( QResizeEvent *);
        void adjustSize();


   signals:
        void selectionChanged(Object*);
        void paintFinished();

   private slots:
        void onCustomContextMenuRequested(const QPoint&);
        void onMoveUpTriggered();
        void onMoveDownTriggered();
        void onMoveFillWidthTriggered();
        void onClearBackgroundTriggered();
        void onCopyTriggered();
        void onCutTriggered();
        void onPasteTriggered();
        void onDeleteTriggered();
        void onEditObjectTriggered();
        void onCancelEditObjectTriggered();
        void onResize(const QResizeEvent&);
        void onObjectDestroyed();
        void alignObject();

   private:
        void performOperation(Clipboard::Operation);

};


#endif
