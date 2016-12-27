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

#include "drawing_surface_widget.h"

#include <QDebug>
#include <QFont>
#include <QFontDatabase>
#include <QMenu>
#include <QLayout>

#include "textbox.h"
#include "scene.h"
#include "objectgroup.h"

static QWidget *mInstance = 0;

DrawingSurfaceWidget::DrawingSurfaceWidget(QWidget *parent) :
    QWidget(parent)
{
    mMousePressed = false;
    mResizing = false;
    mCanResize = false;
    mMoving = false;
    mCanMove = false;
    mSceneManager = 0;
    setContextMenuPolicy(Qt::CustomContextMenu);

    setFixedSize(QSize(Scene::width(), Scene::height()));
    setMouseTracking (true);

    mInstance = this;
    mObject = 0;

    /*QWidget *widget = new QWidget(this);
    widget->setFixedHeight(Scene::height());
    widget->setFixedWidth(Scene::width());
    //this->setWidget(widget);*/

    //create context menu actions
    mCancelEditObject = new QAction(QIcon(":/media/object-edit.png"), tr("Cancel edit"), this);
    mEditObject = new QAction(QIcon(":/media/object-edit.png"), tr("Edit"), this);

    mMoveUp = new QAction(QIcon(":/media/go-up.png"), tr("Move to front"), this);
    mMoveDown = new QAction(QIcon(":/media/go-down.png"), tr("Move to back"), this);
    mFillWidth = new QAction(QIcon(":/media/fit-width.png"), tr("Fill scene's width"), this);

    mCopyObject = new QAction(QIcon(":/media/editcopy.png"), tr("Copy"), parent);
    mCopyObject->setShortcut(QKeySequence("Ctrl+C"));
    mCopyObject->setShortcutContext(Qt::WidgetShortcut);
    parent->addAction(mCopyObject);

    mCutObject = new QAction(QIcon(":/media/editcut.png"), tr("Cut"), parent);
    mCutObject->setShortcut(QKeySequence("Ctrl+X"));
    mCutObject->setShortcutContext(Qt::WidgetShortcut);
    parent->addAction(mCutObject);

    mPasteObject = new QAction(QIcon(":/media/editpaste.png"), tr("Paste"), parent);
    mPasteObject->setShortcut(QKeySequence("Ctrl+V"));
    mPasteObject->setShortcutContext(Qt::WidgetShortcut);
    parent->addAction(mPasteObject);

    mDeleteObject = new QAction(QIcon(":/media/delete.png"), tr("Remove"), parent);
    mDeleteObject->setShortcut(QKeySequence::Delete);
    mDeleteObject->setShortcutContext(Qt::WidgetShortcut);
    parent->addAction(mDeleteObject);

    //Align horizontally
    mAlignHorizontally = new QAction(QIcon(":/media/align-horizontal-center.png"), tr("Align Horizontally"), parent);
    QMenu *menu = new QMenu("Align Horizontally", parent);

    //setup horizontal align actions
    QAction* alignLeft = new QAction(QIcon(":/media/align-horizontal-left.png"), tr("Align Left"), menu);
    QAction* alignHCenter = new QAction(QIcon(":/media/align-horizontal-center.png"), tr("Align Center"), menu);
    QAction* alignRight = new QAction(QIcon(":/media/align-horizontal-right.png"), tr("Align Right"), menu);
    alignLeft->setObjectName("alignLeft");
    alignHCenter->setObjectName("alignHCenter");
    alignRight->setObjectName("alignRight");

    connect(alignLeft, SIGNAL(triggered()), this, SLOT(alignObject()));
    connect(alignHCenter, SIGNAL(triggered()), this, SLOT(alignObject()));
    connect(alignRight, SIGNAL(triggered()), this, SLOT(alignObject()));

    menu->addAction(alignLeft);
    menu->addAction(alignHCenter);
    menu->addAction(alignRight);
    mAlignHorizontally->setMenu(menu);
    parent->addAction(mAlignHorizontally);

    //Align vertically
    mAlignVertically = new QAction(QIcon(":/media/align-vertical-center.png"), tr("Align Vertically"), parent);
    menu = new QMenu("Align Vertically", parent);

    //setup vertical align actions
    QAction* alignTop = new QAction(QIcon(":/media/align-vertical-top.png"), tr("Align Top"), menu);
    QAction* alignVCenter = new QAction(QIcon(":/media/align-vertical-center.png"), tr("Align Center"), menu);
    QAction* alignBottom = new QAction(QIcon(":/media/align-vertical-bottom.png"), tr("Align Bottom"), menu);
    alignTop->setObjectName("alignTop");
    alignVCenter->setObjectName("alignVCenter");
    alignBottom->setObjectName("alignBottom");

    connect(alignTop, SIGNAL(triggered()), this, SLOT(alignObject()));
    connect(alignVCenter, SIGNAL(triggered()), this, SLOT(alignObject()));
    connect(alignBottom, SIGNAL(triggered()), this, SLOT(alignObject()));

    menu->addAction(alignTop);
    menu->addAction(alignVCenter);
    menu->addAction(alignBottom);
    mAlignVertically->setMenu(menu);
    parent->addAction(mAlignVertically);

    mClearBackground = new QAction(tr("Clear Background"), this);

    connect(mEditObject, SIGNAL(triggered()), this, SLOT(onEditObjectTriggered()));
    connect(mCancelEditObject, SIGNAL(triggered()), this, SLOT(onCancelEditObjectTriggered()));
    connect(mMoveUp, SIGNAL(triggered()), this, SLOT(onMoveUpTriggered()));
    connect(mMoveDown, SIGNAL(triggered()), this, SLOT(onMoveDownTriggered()));
    connect(mFillWidth, SIGNAL(triggered()), this, SLOT(onMoveFillWidthTriggered()));
    connect(mCopyObject, SIGNAL(triggered()), this, SLOT(onCopyTriggered()));
    connect(mCutObject, SIGNAL(triggered()), this, SLOT(onCutTriggered()));
    connect(mPasteObject, SIGNAL(triggered()), this, SLOT(onPasteTriggered()));
    connect(mDeleteObject, SIGNAL(triggered()), this, SLOT(onDeleteTriggered()));
    connect(mClearBackground, SIGNAL(triggered()), this, SLOT(onClearBackgroundTriggered()));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onCustomContextMenuRequested(const QPoint&)));
}

DrawingSurfaceWidget::~DrawingSurfaceWidget()
{
}

bool DrawingSurfaceWidget::eventFilter(QObject * obj, QEvent * event)
{
    if (mObject && event->type() == QEvent::Resize && obj == parentWidget()) {
        adjustSize();
    }

    return QObject::eventFilter(obj, event);
}

void DrawingSurfaceWidget::paintEvent(QPaintEvent* paint)
{

    if (mObject)
        paintObject(this);
    else if (mSceneManager && mSceneManager->currentScene())
        paintSceneTo(this);
    else
        return;

    emit paintFinished();
}

void DrawingSurfaceWidget::paintObject(QPaintDevice * paintDevice)
{
    if(! mObject)
        return;

    QPainter painter(paintDevice);
    painter.fillRect(0, 0, width(), height(), Qt::gray);
    mObject->paint(painter);
    drawSelection(painter, mObject->selectedObject());
}

void DrawingSurfaceWidget::setObject(Object* obj)
{
    if (mObject)
        mObject->disconnect(this);

    mObject = obj;
    if (mObject) {
        adjustSize();
        update();
        connect(mObject, SIGNAL(dataChanged()), this, SLOT(update()));
        connect(mObject, SIGNAL(destroyed()), this, SLOT(onObjectDestroyed()));
    }
    else {
        setFixedSize(QSize(Scene::width(), Scene::height()));
    }
}

Object* DrawingSurfaceWidget::object()
{
    return mObject;
}

void DrawingSurfaceWidget::paintSceneTo(QPaintDevice * paintDevice)
{
    Scene *scene = mSceneManager->currentScene();

    if (! scene || ! paintDevice)
        return;

    QPainter painter(paintDevice);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    scene->paint(painter);
    drawSelection(painter, scene->selectedObject());
}

void DrawingSurfaceWidget::drawSelection(QPainter& painter, Object* object)
{
    if (! object)
        return;

    painter.save();
    QPen pen(Qt::black);
    painter.setPen(pen);

    QRectF rectf = object->sceneRect();
    painter.drawRect(rectf);

    QBrush brush(QColor(255,0,0));
    QList<QRect> rects = object->resizeRects();
    foreach(const QRect& rect, rects) {
        painter.fillRect(rect, brush);
        painter.drawRect(rect);
    }

    painter.restore();
}


void DrawingSurfaceWidget::mousePressEvent ( QMouseEvent * event )
{
    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }

    qreal x = event->x(), y = event->y();
    Object * object = 0;
    mMousePressed = true;

    if (mObject) {
        mObject->selectObjectAt(x, y);
        object = mObject->selectedObject();
    }
    else if (mSceneManager->currentScene()) {
        mSceneManager->currentScene()->selectObjectAt(x, y);
        object = mSceneManager->currentScene()->selectedObject();
    }

    if (mCanResize)
        mResizing = true;
    else if (mCanMove) {
        setCursor(Qt::ClosedHandCursor);
        mMoving = true;
    }

    if (object)
        emit selectionChanged(object);
    else if (mObject)
        emit selectionChanged(mObject);
    else
        emit selectionChanged(0);

    update();

    QWidget::mousePressEvent(event);
}

void DrawingSurfaceWidget::mouseReleaseEvent ( QMouseEvent * event)
{
    bool resizing = mResizing;
    bool moving = mMoving;
    Object* object = selectedObject();

    mMousePressed = false;
    mResizing = false;
    mCanResize = false;
    mMoving = false;

    if (object) {
        if (resizing)
            object->stopResizing();
        if (moving)
            object->stopMove();
        if (object->contains(event->x(), event->y()))
            setCursor(Qt::OpenHandCursor);
        else
            setCursor(Qt::ArrowCursor);
    }

    QWidget::mouseReleaseEvent(event);
}

void DrawingSurfaceWidget::mouseMoveEvent( QMouseEvent * event)
{
    Object* object = selectedObject();
    qreal x = event->x(), y = event->y();
    int i;

    //if moving or resizing an object
    if (object && (mResizing || mMoving)) {
        if (mResizing) {
            object->resize(x, y);
        }
        else if (mMoving)
            object->dragMove(x, y);
        update();
        return;
    }

    //get hovered object at x, y, if any
    object = objectAt(x, y);
    mCanMove = false;
    mCanResize = false;

    if (! object) {
        if (cursor().shape() != Qt::ArrowCursor)
            setCursor(Qt::ArrowCursor);
        return;
    }

    QList<QRect> rects =  object->resizeRects();
    for(i=0; i < rects.size(); i++) {
        if (rects.at(i).contains(x, y))
            break;
    }

    object->setHoveredResizeRect(i);

    //if cursor is not hovering one of the rectangles, move the object
    if (i == rects.size()) {
        mCanMove = object->contains(x, y);
        if (mCanMove)
            setCursor(Qt::OpenHandCursor);
        else
            setCursor(Qt::ArrowCursor);
    }
    else {

        if (i % 2 == 0) {
            if (i == 0 || i == 4)
                setCursor(Qt::SizeFDiagCursor);
            else
                setCursor(Qt::SizeBDiagCursor);
        }
        else {
            if (i == 1 || i == 5)
                setCursor(Qt::SizeVerCursor);
            else
                setCursor(Qt::SizeHorCursor);
        }

        mCanResize = true;
    }
}

void DrawingSurfaceWidget::resizeEvent(QResizeEvent * event)
{
    QWidget::resizeEvent(event);
    setFixedSize(event->size());
    //mSceneManager->onResizeEvent(event);
}

void DrawingSurfaceWidget::onCustomContextMenuRequested(const QPoint& point)
{
    if (! mSceneManager->currentScene() && ! mObject)
        return;

    QMenu menu;
    bool inScene = mObject ? false : true;
    Object* selectedObject = 0;
    if (mObject)
        selectedObject = mObject->selectedObject();
    else
        selectedObject = mSceneManager->currentScene()->selectedObject();

    if (selectedObject) {
        ObjectGroup * objectGroup = qobject_cast<ObjectGroup*>(selectedObject);
        if (objectGroup) {
            if (objectGroup->editingMode())
                menu.addAction(mCancelEditObject);
            else
                menu.addAction(mEditObject);
        }
        else if (selectedObject->hasObjectAsParent())
            menu.addAction(mCancelEditObject);

        if (inScene) {
            menu.addAction(mMoveUp);
            menu.addAction(mMoveDown);
            menu.addAction(mFillWidth);
            menu.addSeparator();

            menu.addAction(mAlignHorizontally);
            menu.addAction(mAlignVertically);
            menu.addSeparator();

            menu.addAction(mCopyObject);
            menu.addAction(mCutObject);
            if (! mSceneManager->clipboard()->isEmpty())
                menu.addAction(mPasteObject);
            menu.addAction(mDeleteObject);
        }
    }
    else if (inScene) {
        if (! mSceneManager->clipboard()->isEmpty())
            menu.addAction(mPasteObject);
        if (mSceneManager->currentScene()->backgroundImage())
            menu.addAction(mClearBackground);
    }

    if (! menu.actions().isEmpty()) {
        if (cursor().shape() != Qt::ArrowCursor)
            setCursor(Qt::ArrowCursor);
        menu.exec(mapToGlobal(point));
    }

    mMousePressed = false;
}

void DrawingSurfaceWidget::onMoveUpTriggered()
{
    if (mSceneManager->currentScene())
        mSceneManager->currentScene()->moveSelectedObjectUp();
}

void DrawingSurfaceWidget::onMoveDownTriggered()
{
    if (mSceneManager->currentScene())
        mSceneManager->currentScene()->moveSelectedObjectDown();
}

void DrawingSurfaceWidget::onMoveFillWidthTriggered()
{
    if (mSceneManager->currentScene())
        mSceneManager->currentScene()->fillWidth();
}

void DrawingSurfaceWidget::onCopyTriggered()
{
    performOperation(Clipboard::Copy);
}

void DrawingSurfaceWidget::onCutTriggered()
{
    performOperation(Clipboard::Cut);
}

void DrawingSurfaceWidget::performOperation(Clipboard::Operation op)
{
    if (mSceneManager->currentScene() && mSceneManager->currentScene()->selectedObject()) {
        mSceneManager->clipboard()->clear();
        mSceneManager->clipboard()->add(mSceneManager->currentScene()->selectedObject(), op);
    }
}

void DrawingSurfaceWidget::onPasteTriggered()
{
    Scene * destScene = mSceneManager->currentScene();

    if (destScene) {
        QList<Object*> objects = mSceneManager->clipboard()->objects();
        Clipboard::Operation op = mSceneManager->clipboard()->operation();
        Scene * srcScene = 0;

        foreach(Object* obj, objects) {
            srcScene = obj->scene();

            if (op == Clipboard::Copy) {
                obj = obj->copy();
                destScene->appendObject(obj);
            }
            else if (op == Clipboard::Cut && srcScene && srcScene != destScene) {
                srcScene->removeObject(obj);
                destScene->appendObject(obj);
            }
        }

        mSceneManager->clipboard()->clear();
    }
}

void DrawingSurfaceWidget::onDeleteTriggered()
{
    if (mSceneManager->currentScene() && mSceneManager->currentScene()->selectedObject()) {
        mSceneManager->clipboard();
        mSceneManager->currentScene()->removeSelectedObject(true);
    }
}

void DrawingSurfaceWidget::onClearBackgroundTriggered()
{
    if (mSceneManager->currentScene())
        mSceneManager->currentScene()->clearBackground();
}

void DrawingSurfaceWidget::onEditObjectTriggered()
{
    if (mObject || (mSceneManager->currentScene() && mSceneManager->currentScene()->selectedObject())) {
        Object* obj = mObject ? mObject : mSceneManager->currentScene()->selectedObject();
        ObjectGroup *objGroup = qobject_cast<ObjectGroup*>(obj);
        if (objGroup)
            objGroup->setEditingMode(true);
    }
}

void DrawingSurfaceWidget::onCancelEditObjectTriggered()
{
    if (mObject || (mSceneManager->currentScene() && mSceneManager->currentScene()->selectedObject())) {
        Object* selectedObject = mObject ? mObject : mSceneManager->currentScene()->selectedObject();
        ObjectGroup *objGroup = qobject_cast<ObjectGroup*>(selectedObject);

        if (! objGroup && selectedObject->parent()){
            objGroup = qobject_cast<ObjectGroup*>(selectedObject->parent());
        }

        if (objGroup)
            objGroup->setEditingMode(false);
    }
}

QWidget * DrawingSurfaceWidget::instance()
{
    return mInstance;
}

void DrawingSurfaceWidget::onResize(const QResizeEvent & event)
{
    this->setFixedSize(event.size());
}

void DrawingSurfaceWidget::adjustSize()
{
    if (! mObject)
        return;

    int w = parentWidget() ? parentWidget()->width() : width();
    int h = parentWidget() ? parentWidget()->height() : height();

    if (mObject->width() > w)
        w = mObject->width() + MARGIN*2;
    if (mObject->height() > h)
        h = mObject->height() + MARGIN*2;

    setFixedSize(w, h);
    mObject->setX(w / 2 - mObject->width() / 2);
    mObject->setY(h / 2 - mObject->height() / 2);
}

Object* DrawingSurfaceWidget::objectAt(qreal x, qreal y)
{
    if (mObject)
        return mObject->objectAt(x, y);
    else if (mSceneManager && mSceneManager->currentScene())
       return mSceneManager->currentScene()->objectAt(x, y);
    return 0;
}

Object* DrawingSurfaceWidget::selectedObject()
{
    if (mObject)
        return mObject->selectedObject();
    else if (mSceneManager && mSceneManager->currentScene())
       return mSceneManager->currentScene()->selectedObject();
    return 0;
}

void DrawingSurfaceWidget::onObjectDestroyed()
{
    mObject = 0;
    setObject(0);
}

void DrawingSurfaceWidget::setSceneManager(SceneManager* sceneManager)
{
    if (mSceneManager == sceneManager)
        return;

    mSceneManager = sceneManager;
}

void DrawingSurfaceWidget::alignObject() {
    if (! sender())
        return;

    Object* object = selectedObject();
    if (! object)
        return;

    QString action = sender()->objectName();

    if (action == "alignLeft") {
        object->setX(0);
    }
    else if (action == "alignHCenter") {
        object->setX(Scene::width()/2 - object->width()/2);
    }
    else if (action == "alignRight") {
        object->setX(Scene::width() - object->width());
    }
    else if (action == "alignTop") {
        object->setY(0);
    }
    else if (action == "alignVCenter") {
        object->setY(Scene::height()/2 - object->height()/2);
    }
    else if (action == "alignBottom") {
        object->setY(Scene::height()-object->height());
    }
}
