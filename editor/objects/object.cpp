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

#include "objects/object.h"

#include <QFile>
#include <QColor>
#include <QFont>
#include <math.h>

#include "scene.h"
#include "utils.h"
#include "resource_manager.h"
#include "gameobjectfactory.h"

static QFont mDefaultFont;

Object::Object(QObject* parent, const QString& name):
    GameObject(parent, name)
{
    init();
    updateResizeRects();
    emit dataChanged();
}

Object::Object(const QVariantMap& data, QObject* parent):
    GameObject(data, parent)
{
    init();
    loadInternal(data);
    updateResizeRects();
    emit dataChanged();
}

void Object::init()
{
    setType(GameObjectMetaType::Object);
    mOpacity = 255;
    mBackground.setOpacity(255);
    mTemporaryBackground.setOpacity(255);
    mCornerRadius = 0;
    mOriginalResizePointIndex = -1;
    mVisible = true;
    mPercentWidth = 0;
    mPercentHeight = 0;
    mBorderWidth = 0;
    mBorderColor = QColor();
    mSelectedObject = 0;
    mKeepAspectRatio = false;
    mAspectRatio = 1;
    mScaledBackgroundImage = 0;
    initEventActionManager(Interaction::MouseMove);
    initEventActionManager(Interaction::MousePress);
    initEventActionManager(Interaction::MouseRelease);
}

Object::~Object()
{
    emit destroyed(this);

    if (mBackground.image())
        AssetManager::instance()->releaseAsset(mBackground.image());
}

void Object::initEventActionManager(Interaction::InputEvent event)
{
    if (mEventToActions.contains(event))
        return;

    GameObjectManager* actionManager = new GameObjectManager(this);
    actionManager->setUniqueNames(false);
    actionManager->setAllowEmptyNames(true);
    actionManager->setObjectsParent(this);
    actionManager->setProperty("event", event);
    connect(actionManager, SIGNAL(objectInserted(int,GameObject*)), this, SLOT(onEventActionInserted(int, GameObject*)));
    connect(actionManager, SIGNAL(objectRemoved(GameObject*, bool)), this, SLOT(onEventActionRemoved(GameObject*, bool)));
    connect(actionManager, SIGNAL(objectMoved(GameObject*, int)), this, SLOT(onEventActionMoved(GameObject*, int)));
    mEventToActions.insert(event, actionManager);
}

bool Object::contains(qreal x, qreal y)
{
    if (mSceneRect.contains(x, y) || containsResizeRectAt(x, y))
        return true;
        
    return false;
}

QRect Object::sceneRect() const
{
    return mSceneRect;
}

QRect Object::contentRect() const
{
    QRect rect;
    rect.setX(contentX());
    rect.setY(contentY());
    rect.setWidth(contentWidth());
    rect.setHeight(contentHeight());
    return rect;
}

void Object::onResizeEvent(QResizeEvent* event)
{
    return;

    int width = Scene::width() - mSceneRect.width();
    int height = Scene::height() - mSceneRect.height();
    
    mSceneRect.setWidth(mSceneRect.width() + width);
    mSceneRect.setHeight(mSceneRect.height() + height);
}

void Object::move(int x, int y)
{
    mSceneRect.moveTo(x, y);
    updateResizeRects();
    QVariantMap data;
    data.insert("x", x);
    data.insert("y", y);
    notify(data);
    //emit positionChanged(mSceneRect.x(), mSceneRect.y());
}

void Object::dragMove(int x, int y)
{
    if (mPreviousSceneRect.isEmpty() || mPreviousSceneRect.isNull()) {
        mPreviousSceneRect = mSceneRect;
        mXDif = x - mSceneRect.x();
        mYDif = y - mSceneRect.y();
    }
    
    //since x and y represent mouse coordinates and we move the rectangle by moving its top left corner,
    //we need to know the initial distance from the point where the mouse was clicked to the top left corner.
    move(x - mXDif, y - mYDif);
}

void Object::stopMove()
{
    mPreviousSceneRects.append(mPreviousSceneRect);
    mPreviousSceneRect = QRect();
}

QList<QRect> Object::previousSceneRects() const
{
    return mPreviousSceneRects;
}

QColor Object::backgroundColor() const
{
    return mBackground.color();
}

void Object::setBackgroundColor(const QColor & color)
{
    mBackground.setColor(color);
    notify("backgroundColor", Utils::colorToList(mBackground.color()));
}

int Object::backgroundOpacity() const
{
    return mBackground.opacity();
}

void Object::setBackgroundOpacity(int alpha)
{
    mBackground.setOpacity(alpha);
    notify("backgroundOpacity", mBackground.opacity());
}

int Object::opacity() const
{
    return mOpacity;
}

qreal Object::opacityF() const
{
    return (mOpacity * 1.0) / 255;
}

void Object::setOpacity(int opacity)
{
    if (opacity > 255)
        opacity = 255;
    else if (opacity < 0)
        opacity = 0;

    if (opacity != mOpacity){
        mOpacity = opacity;
        notify("opacity", mOpacity);
    }
}


void Object::setWidth(int w, bool percent)
{
    int width = mSceneRect.width();

    if (percent) {
        mPercentWidth = w;
        Object * obj = qobject_cast<Object*>(parent());
        if (obj)
            width = obj->width() * w / 100;
        else if (scene())
            width = scene()->width() * w / 100;

        mSceneRect.setWidth(width);
    }
    else {
        mSceneRect.setWidth(w);
        mPercentWidth = 0;
    }

    updateResizeRects();

    notify("width", mSceneRect.width());
    emit resized(mSceneRect.width(), mSceneRect.height());
}

void Object::setHeight(int h, bool percent)
{
    int height = mSceneRect.height();

    if (percent) {
        mPercentHeight = h;
        Object * obj = qobject_cast<Object*>(parent());
        if (obj)
            height = obj->height() * h / 100;
        else if(scene())
            height = scene()->height() * h / 100;

        mSceneRect.setHeight(height);
    }
    else {
        mPercentHeight = 0;
        mSceneRect.setHeight(h);
    }

    updateResizeRects();

    notify("height", mSceneRect.height());
    emit resized(mSceneRect.width(), mSceneRect.height());
}

void Object::setY(int y)
{
    if (y == mSceneRect.y())
        return;

    mSceneRect.moveTo(mSceneRect.x(), y);
    updateResizeRects();
    //FIXME: Implement proper relative positions.
    notify("y", this->y());
}

void Object::setX(int x)
{
    if(x == mSceneRect.x())
        return;

    mSceneRect.moveTo(x, mSceneRect.y());
    updateResizeRects();
    notify("x", this->x());
}

int Object::x() const
{
    return mSceneRect.x();
}

int Object::contentX() const
{
    return mSceneRect.x() + mPadding.left();
}

int Object::y() const
{
    return mSceneRect.y();
}

int Object::contentY() const
{
    return mSceneRect.y() + mPadding.top();
}

int Object::parentWidth() const
{
    if (! parent())
        return 0;
    Object* p = qobject_cast<Object*>(parent());

    if (p)
        return p->width();
    else
        return Scene::width();
}

int Object::width() const
{
    int width = 0;
    if (mPercentWidth)
        width = mPercentWidth / 100 * parentWidth();
    else
        width = mSceneRect.width();

    return width;
}

int Object::contentWidth() const
{
    int width = this->width();
    width -= mPadding.left() + mPadding.right();
    return width;
}

int Object::parentHeight() const
{
    if (! parent())
        return 0;
    Object* p = qobject_cast<Object*>(parent());
    if (p)
        return p->height();
    else
        return Scene::height();
}

int Object::contentHeight() const
{
    int height = this->height();
    height -= mPadding.top() + mPadding.bottom();
    return height;
}

int Object::height() const
{
    int height = 0;

    if (mPercentHeight)
        height = mPercentHeight / 100 * parentHeight();
    else
        height = mSceneRect.height();

    return height;
}

void Object::update()
{
    emit dataChanged();
}

bool Object::isRounded() const
{
    if (mCornerRadius)
        return true;
    return false;
}

void Object::setCornerRadius(int r)
{
    if (mCornerRadius != r) {
        mCornerRadius = r;
        QVariantMap data;
        data.insert("cornerRadius", r);
        emit dataChanged(data);
    }
}

int Object::cornerRadius()
{
    return mCornerRadius;
}

void Object::paint(QPainter & painter)
{
    QRect rect(mSceneRect.x(), mSceneRect.y(), width(), height());
    if (! mVisible || ! mOpacity) {
        QPen pen(Qt::white);
        painter.setOpacity(0.5);
        pen.setWidth(3);
        painter.setPen(pen);
        painter.drawRect(rect);
        painter.setOpacity(0);
        return;
    }

    qreal opacity =  opacityF() * painter.opacity();
    painter.setOpacity(opacity);
    if (mTemporaryBackground.isValid())
        mTemporaryBackground.paint(painter, rect, mCornerRadius, opacity);
    else
        mBackground.paint(painter, rect, mCornerRadius, opacity);

    if (mBorderColor.isValid()) {
        if (mBorderWidth)
            painter.setPen(QPen(mBorderColor, mBorderWidth));
        else
            painter.setPen(Qt::NoPen);
        if (mCornerRadius)
            painter.drawRoundedRect(rect, mCornerRadius, mCornerRadius);
        else
            painter.drawRect(rect);
    }
}

Object* Object::copy()
{
    return ResourceManager::instance()->createObject(toJsonObject(true), this->parent());
}

void Object::addEventActions(Interaction::InputEvent event, const QVariantList & actions)
{
    QList<Action*> newActions;
    Action* action = 0;
    QVariantMap actionData;

    for(int i=0; i < actions.size(); i++) {
        if (actions[i].type() == QVariant::Map) {
            actionData = actions[i].toMap();
            action = GameObjectFactory::createAction(actionData, this);
            newActions.append(action);
        }
        else if (actions[i].type() == QMetaType::QObjectStar) {
            action = actions[i].value<Action*>();
            if (action) {
                action = GameObjectFactory::createAction(action->toJsonObject(), this);
                if (action)
                    newActions.append(action);
            }
        }
    }

    replaceEventActions(event, newActions);
}

void Object::clearEventActions(Interaction::InputEvent event, const QList<Action *> & newActions)
{
    GameObjectManager* actionManager = mEventToActions.value(event);
    Action* action = 0;

    if (!actionManager)
        return;

    for(int i=actionManager->size()-1; i >= 0; i--) {
        action = qobject_cast<Action*>(actionManager->objectAt(i));
        if (newActions.contains(action))
            removeEventActionAt(event, i, false);
        else
            removeEventActionAt(event, i, true);
    }

    actionManager->clear();
}

void Object::replaceEventActions(Interaction::InputEvent event, const QList<Action*>& actions)
{
    clearEventActions(event, actions);
    GameObjectManager* actionManager = mEventToActions.value(event);
    if (actionManager) {
        actionManager->clear();
        foreach(Action* action, actions) {
            actionManager->add(action);
        }
    }
}

void Object::setEventActions(Interaction::InputEvent event, const QList<Action*>& actions)
{
    replaceEventActions(event, actions);
}

void Object::appendEventAction(Interaction::InputEvent event, Action * action)
{
    GameObjectManager* actionManager = mEventToActions.value(event);
    if (actionManager) {
        insertEventAction(event, actionManager->size(), action);
    }
}

void Object::insertEventAction(Interaction::InputEvent event, int index, Action * action)
{
    GameObjectManager* actionManager = mEventToActions.value(event);

    if (action && actionManager) {
        Action* newAction = 0;
        GameObject* resource = this->resource();
        //copy action if it comes from the resource
        if (resource && action->parent() == resource) {
            newAction = GameObjectFactory::createAction(action->toJsonObject(), this);
            newAction->setResource(action);
            connect(this, SIGNAL(syncChanged(bool)), newAction, SLOT(setSync(bool)));
            action = newAction;
        }

        if (! action->parent() || action->parent() != this) {
            action->setParent(this);
        }

        actionManager->insert(index, action);

        if (isSynced() && isResource()) {
            emit eventActionInserted(event, index, action);
        }
    }
}

void Object::removeEventActionAt(Interaction::InputEvent event, int index, bool del)
{
   if (loadBlocked())
       return;

   GameObjectManager* actionManager = mEventToActions.value(event);
   if (!actionManager)
       return;

   GameObject* obj = actionManager->takeAt(index);
   Action* action = qobject_cast<Action*>(obj);

   if (action) {
       bool loadBlocked = blockLoad(true);
       emit eventActionRemoved(event, action, del);
       blockLoad(loadBlocked);

       if (del && action && action->parent() == this)
           delete action;
   }
}

void Object::removeEventAction(Interaction::InputEvent event, Action* action, bool del)
{
    if (! action)
        return;

    GameObjectManager* actionManager = mEventToActions.value(event);
    if (actionManager)
        this->removeEventActionAt(event, actionManager->indexOf(action), del);
}

void Object::removeEventActions(Interaction::InputEvent event, bool del)
{
    GameObjectManager* actionManager = mEventToActions.value(event);
    if (actionManager)
        actionManager->clear(del);
}

void Object::removeEventActionSync(Interaction::InputEvent event, Action * action, bool del)
{
    if (!action || loadBlocked())
        return;

    if (isResource()) {
        removeEventAction(event, qobject_cast<Action*>(action->resource()), del);
    }
    else {
        QList<Action*> actions = actionsForEvent(event);
        foreach(Action* _action, actions) {
            if (_action && _action->resource() == action)
                removeEventAction(event, _action, del);
        }
    }
}

void Object::moveEventActionSync(Interaction::InputEvent event, Action* action, int to)
{
    if (!action || loadBlocked())
        return;

    GameObjectManager* actionManager = actionManagerForEvent(event);
    if (!actionManager)
        return;

    if (isResource()) {
        actionManager->move(action->resource(), to);
    }
    else {
        QList<GameObject*> actions = actionManager->objects();
        bool moved;
        foreach(GameObject* _action, actions) {
            if (_action && _action->resource() == action) {
                moved = actionManager->move(_action, to);
                if (moved)
                    break;
            }
        }
    }
}

void Object::setBackgroundImage(const QString & path)
{
    ImageFile* image = mBackground.image();

    if (image && image->path() == path)
        return;

    if (image && image->isAnimated()) {
        image->movie()->stop();
        image->movie()->disconnect(this);
    }

    AssetManager::instance()->releaseAsset(image);

    image = dynamic_cast<ImageFile*>(AssetManager::instance()->loadAsset(path, Asset::Image));
    if (image && image->isAnimated()) {
        connect(image->movie(), SIGNAL(frameChanged(int)), this, SIGNAL(dataChanged()));
        image->movie()->start();
    }

    mBackground.setImage(image);
    notify("backgroundImage", path);
}

ImageFile* Object::backgroundImage() const
{
    return mBackground.image();
}

QList<Action*> Object::actionsForEvent(Interaction::InputEvent event) const
{
    GameObjectManager* actionManager = mEventToActions.value(event);
    QList<Action*> actions;
    Action* action = 0;

    if (!actionManager)
        return actions;

    foreach(GameObject* obj, actionManager->objects()) {
        action = qobject_cast<Action*>(obj);
        if (action)
            actions.append(action);
    }

    return actions;
}

GameObjectManager* Object::actionManagerForEvent(Interaction::InputEvent event) const
{
    return mEventToActions.value(event);
}

Action* Object::eventAction(Interaction::InputEvent event, const QString& name)
{
    QList<Action*> actions = actionsForEvent(event);
    for(int i=0; i < actions.size(); i++) {
        if (actions[i]->name() == name)
            return actions[i];
    }

    return 0;
}

bool Object::hasActionForEvent(Action* action, Interaction::InputEvent event)
{
    GameObjectManager* actionManager = mEventToActions.value(event);
    if (actionManager)
        return actionManager->contains(action);
    return false;
}

QVariantMap Object::toJsonObject(bool internal) const
{
    QVariantMap object = GameObject::toJsonObject(internal);
    if (!internal || !isResource()) {
        object.insert("x", x());
        object.insert("y", y());
    }
    object.insert("opacity", mOpacity);

    if (mPercentWidth) object.insert("width", QString("%1\%").arg(mPercentWidth));
    else object.insert("width", width());
    if (mPercentHeight) object.insert("height", QString("%1\%").arg(mPercentHeight));
    else object.insert("height", height());
    object.insert("backgroundColor", Utils::colorToList(mBackground.color()));
    object.insert("backgroundOpacity", mBackground.opacity());

    if (mCornerRadius) {
        object.insert("cornerRadius", mCornerRadius);
    }

    if (mBackground.image()) {
        object.insert("backgroundImage", mBackground.image()->name());
    }

    QVariantList jsonActions;
    QList<Action*> actions = actionsForEvent(Interaction::MousePress);
    if (internal) {
        for(int i=0; i < actions.size(); i++)
            jsonActions.append(QVariant::fromValue(qobject_cast<QObject*>(actions[i])));
    }
    else {
        for(int i=0; i < actions.size(); i++)
            jsonActions.append(actions[i]->toJsonObject(internal));
    }

    if (jsonActions.size() || internal)
        object.insert("onMousePress", jsonActions);

    jsonActions.clear();
    actions = actionsForEvent(Interaction::MouseRelease);
    if (internal) {
        for(int i=0; i < actions.size(); i++)
            jsonActions.append(QVariant::fromValue(qobject_cast<QObject*>(actions[i])));
    }
    else {
        for(int i=0; i < actions.size(); i++)
            jsonActions.append(actions[i]->toJsonObject(internal));
    }

    if (jsonActions.size() || internal)
        object.insert("onMouseRelease", jsonActions);

    jsonActions.clear();
    actions = actionsForEvent(Interaction::MouseMove);

    if (internal) {
        for(int i=0; i < actions.size(); i++)
            jsonActions.append(QVariant::fromValue(qobject_cast<QObject*>(actions[i])));
    }
    else {
        for(int i=0; i < actions.size(); i++)
            jsonActions.append(actions[i]->toJsonObject(internal));
    }

    if (jsonActions.size() || internal)
        object.insert("onMouseMove", jsonActions);

    object.insert("visible", mVisible);
    if (! mPadding.isEmpty())
        object.insert("padding", mPadding.toJsonObject());
    if (mBorderWidth > 0 || internal)
        object.insert("borderWidth", mBorderWidth);
    if (mBorderColor.isValid() || internal)
        object.insert("borderColor", Utils::colorToList(mBorderColor));

    object.insert("keepAspectRatio", mKeepAspectRatio);

    //remove attributes that are the same in the resource
    filterResourceData(object);

    return object;
}

void Object::filterResourceData(QVariantMap& objectData) const
{
    GameObject* res = resource();
    if (! res)
        return;

    objectData.insert("resource", res->name());
    QVariantMap resourceData = res->toJsonObject(false);

    QStringList keys = objectData.keys();
    foreach(const QString& key, keys) {
        if (resourceData.contains(key) && resourceData.value(key) == objectData.value(key))
            objectData.remove(key);
    }
}

void Object::resizeSceneRect(int x, int y)
{
    if (mOriginalResizePointIndex == -1)
        return;

    QPoint point (x, y);
    movePoint(mOriginalResizePointIndex, point);

    if (mKeepAspectRatio) {
        int w2 = width();
        int h2 = round(w2/mAspectRatio);
        fixPoint(mOriginalResizePointIndex, w2, h2);
    }
}

void Object::resize(int pointIndex, int x, int y)
{
    //QPoint point = rect.center();
    QPoint toPoint(x, y);
    mOriginalResizePointIndex = pointIndex;
    movePoint(pointIndex, toPoint);
    updateResizeRects();

    //if (i < 8)
      //  updateResizeRects();
    //if (i < 8)
    //    emit dataChanged();
}

void Object::resize(int x, int y)
{
    if (mOriginalResizePointIndex == -1)
        return;

    resizeSceneRect(x, y);
    updateResizeRects();
    QVariantMap data;
    int w = width();
    int h = height();
    data.insert("width", w);
    data.insert("height", h);
    notify(data);
    emit resized(w, h);
}

void Object::fixPoint(int pindex, int w, int h)
{
    QPoint point;
    int x, y;

    switch (pindex){
        case 0:
            point = mSceneRect.bottomRight();
            x = point.x() - w;
            y = point.y() - h;
            mSceneRect.setTopLeft(QPoint(x, y));
            break;
        case 2:
            point = mSceneRect.bottomLeft();
            y = point.y() - h;
            x = point.x() + w;
            mSceneRect.setTopRight(QPoint(x, y));
            break;

        case 4:
            point = mSceneRect.topLeft();
            y = point.y() + h;
            x = point.x() + w;
            mSceneRect.setBottomRight(QPoint(x, y));
            break;

        case 6:
            point = mSceneRect.topRight();
            y = point.y() + h;
            x = point.x() - w;
            mSceneRect.setBottomLeft(QPoint(x, y));
            break;
    }
}

void Object::movePoint(int which, QPoint& point)
{
    QPoint point2;

    switch(which)
    {
    case 0:
        point2 = mSceneRect.bottomRight();
        if (point.x() > point2.x())
            point.setX(point2.x());
        if (point.y() > point2.y())
            point.setY(point2.y());
        mSceneRect.setTopLeft(point);
        break;
    case 1:
         point2 = mSceneRect.bottomLeft();
         if (point.y() < point2.y())
            mSceneRect.setY(point.y());
         break;
    case 2:
        point2 = mSceneRect.bottomLeft();
        if (point.x() < point2.x())
            point.setX(point2.x());
        if (point.y() > point2.y())
            point.setY(point2.y());
        mSceneRect.setTopRight(point);
        break;
    case 3:
        point2 = mSceneRect.bottomLeft();
        if (point.x() > point2.x())
            mSceneRect.setWidth(point.x()-mSceneRect.x());
        break;
    case 4:
        point2 = mSceneRect.topLeft();
        if (point.x() < point2.x())
            point.setX(point2.x());
        if (point.y() < point2.y())
            point.setY(point2.y());
        mSceneRect.setBottomRight(point);
        break;
    case 5:
        point2 = mSceneRect.topRight();
        if (point.y() > point2.y())
            mSceneRect.setHeight(point.y()-mSceneRect.y());
        break;
    case 6:
        point2 = mSceneRect.topRight();
        if (point.x() > point2.x())
            point.setX(point2.x());
        if (point.y() < point2.y())
            point.setY(point2.y());
        mSceneRect.setBottomLeft(point);
        break;
    case 7:
        point2 = mSceneRect.topRight();
        if (point.x() < point2.x())
            mSceneRect.setX(point.x());
        break;
    }
}

QList<QRect> Object::resizeRects()
{
    return mResizeRects;
}

void Object::updateResizeRect(int i, const QPoint& point)
{
    int rectWidth = RESIZE_RECT_WIDTH;
    int rectWidthHalf = rectWidth/2;

    if (i >= 0 && i < mResizeRects.size()) {
        QRect rect = mResizeRects.at(i);
        rect = QRect(point.x()-rectWidthHalf, point.y()-rectWidthHalf, rectWidth, rectWidth);
        mResizeRects.replace(i, rect);
    }
}

void Object::updateResizeRects()
{
    int rectWidth = RESIZE_RECT_WIDTH;
    int rectWidthHalf = rectWidth/2;

    mResizeRects.clear();
    //top left
    QPoint point = mSceneRect.topLeft();
    QRect rect = QRect(point.x()-rectWidthHalf, point.y()-rectWidthHalf, rectWidth, rectWidth);
    mResizeRects.append(rect);

    //middle top
    point.setX(point.x() + mSceneRect.width()/2);
    rect = QRect(point.x()-rectWidthHalf, point.y()-rectWidthHalf, rectWidth, rectWidth);
    mResizeRects.append(rect);

    //top right
    point = mSceneRect.topRight();
    rect = QRect(point.x()-rectWidthHalf, point.y()-rectWidthHalf, rectWidth, rectWidth);
    mResizeRects.append(rect);

    //middle right
    point.setY(point.y()+mSceneRect.height()/2);
    rect = QRect(point.x()-rectWidthHalf, point.y()-rectWidthHalf, rectWidth, rectWidth);
    mResizeRects.append(rect);

    //bottom right
    point = mSceneRect.bottomRight();
    rect = QRect(point.x()-rectWidthHalf, point.y()-rectWidthHalf, rectWidth, rectWidth);
    mResizeRects.append(rect);

    //middle bottom
    point.setX(point.x() - mSceneRect.width()/2);
    rect = QRect(point.x()-rectWidthHalf, point.y()-rectWidthHalf, rectWidth, rectWidth);
    mResizeRects.append(rect);

    //bottom left
    point = mSceneRect.bottomLeft();
    rect = QRect(point.x()-rectWidthHalf, point.y()-rectWidthHalf, rectWidth, rectWidth);
    mResizeRects.append(rect);

    //middle left
    point.setY(point.y() - mSceneRect.height()/2);
    rect = QRect(point.x()-rectWidthHalf, point.y()-rectWidthHalf, rectWidth, rectWidth);
    mResizeRects.append(rect);
}


QRect Object::resizeRectAt(int x, int y)
{
    foreach(const QRect& rect, mResizeRects) {
        if (rect.contains(x, y))
            return rect;
    }

    return QRect();
}

bool Object::containsResizeRectAt(int x, int y)
{
    foreach(const QRect& rect, mResizeRects) {
        if (rect.contains(x, y))
            return true;
    }

    return false;
}

void Object::setHoveredResizeRect(int i)
{
    mOriginalResizePointIndex = i;
    if (i < 0 || i >= mResizeRects.size())
        mOriginalResizePointIndex = -1;
}

void Object::stopResizing()
{
    mOriginalResizePointIndex = -1;
}

bool Object::visible()
{
    return mVisible;
}

void Object::setVisible(bool visible)
{
    mVisible = visible;
    this->notify("visible", visible);
}

Object* Object::objectAt(qreal x, qreal y)
{
    if (this->contains(x, y))
        return this;
    return 0;
}

void Object::selectObjectAt(qreal x, qreal y)
{
    mSelectedObject = objectAt(x, y);
}

Object* Object::selectedObject()
{
    return mSelectedObject;
}

Padding Object::padding()
{
    return mPadding;
}

int Object::padding(const QString & side)
{
    return mPadding.padding(side);
}

void Object::setPadding(const QString & side, int value){
    Padding padding = mPadding;
    padding.setPadding(side, value);
    setPadding(padding);
}

void Object::setPadding(const Padding& padding)
{
    mPadding = padding;
    notify("padding", mPadding.toJsonObject());
}

int Object::paddingLeft() const
{
    return mPadding.left();
}

void Object::setPaddingLeft(int padding)
{
    mPadding.setLeft(padding);
    notify("padding", mPadding.toJsonObject());
}

int Object::paddingTop() const
{
    return mPadding.top();
}

void Object::setPaddingTop(int padding)
{
    mPadding.setTop(padding);
    notify("padding", mPadding.toJsonObject());
}

int Object::paddingRight() const
{
    return mPadding.right();
}

void Object::setPaddingRight(int padding)
{
    mPadding.setRight(padding);
    notify("padding", mPadding.toJsonObject());
}

int Object::paddingBottom() const
{
    return mPadding.bottom();
}

void Object::setPaddingBottom(int padding)
{
    mPadding.setBottom(padding);
    notify("padding", mPadding.toJsonObject());
}

void Object::moveSharedEventActions(Object* src, Object* dest, Interaction::InputEvent ev)
{
    if (! src || ! dest)
        return;

    QList<Action*> actions;
    actions = src->actionsForEvent(ev);
    for(int i=0; i < actions.size(); i++) {
        if (dest->hasActionForEvent(actions[i], ev))
            actions[i]->setParent(dest);
    }
}

void Object::moveAllSharedEventActions(Object* src, Object* dest)
{
    moveSharedEventActions(src, dest, Interaction::MouseMove);
    moveSharedEventActions(src, dest, Interaction::MousePress);
    moveSharedEventActions(src, dest, Interaction::MouseRelease);
}

bool Object::hasObjectAsParent()
{
    if (! this->parent())
        return false;

    if (qobject_cast<Object*>(this->parent()))
        return true;

    return false;
}

void Object::loadData(const QVariantMap &data, bool internal)
{
    if(!internal)
        GameObject::loadData(data, internal);

    QVariantList eventActions;
    QVariantMap actionData;
    QList<Action*> actions;
    Action* action;

    if (data.contains("name") && data.value("name").type() == QVariant::String)
        setObjectName(data.value("name").toString());

    if (data.contains("opacity") && data.value("opacity").canConvert(QVariant::Int))
        setOpacity(data.value("opacity").toInt());

    if (data.contains("backgroundColor") && data.value("backgroundColor").type() == QVariant::List) {
        QColor color = Utils::listToColor(data.value("backgroundColor").toList());
        setBackgroundColor(color);
    }

    if (data.contains("backgroundOpacity") && data.value("backgroundOpacity").canConvert(QVariant::Int)) {
        setBackgroundOpacity(data.value("backgroundOpacity").toInt());
    }

    if (data.contains("backgroundImage") && data.value("backgroundImage").type() == QVariant::String) {
        setBackgroundImage(data.value("backgroundImage").toString());
    }

    if (data.contains("x") && data.value("x").canConvert(QVariant::Int)) {
        mSceneRect.setX(data.value("x").toInt());
    }

    if (data.contains("y") && data.value("y").canConvert(QVariant::Int))
        mSceneRect.setY(data.value("y").toInt());

    if (data.contains("width")) {
        if (data.value("width").type() == QVariant::String) {
            QString width = data.value("width").toString();
            setWidth(Utils::parseSize(width), Utils::isPercentSize(width));
        }
        else if (data.value("width").canConvert(QVariant::Int))
            setWidth(data.value("width").toInt());
    }

    if (data.contains("height") && data.value("height").canConvert(QVariant::Int)) {
        if (data.value("height").type() == QVariant::String) {
            QString height = data.value("height").toString();
            setHeight(Utils::parseSize(height), Utils::isPercentSize(height));
        }
        else if (data.value("height").canConvert(QVariant::Int))
            setHeight(data.value("height").toInt());
    }

    if (data.contains("cornerRadius") && data.value("cornerRadius").canConvert(QVariant::Int)) {
        setCornerRadius(data.value("cornerRadius").toInt());
    }

    if (data.contains("visible") && data.value("visible").type() == QVariant::Bool)
        setVisible(data.value("visible").toBool());


    if (data.contains("onMouseMove") && data.value("onMouseMove").type() == QVariant::List) {
        addEventActions(Interaction::MouseMove, data.value("onMouseMove").toList());
    }

    if (data.contains("onMousePress") && data.value("onMousePress").type() == QVariant::List) {
        addEventActions(Interaction::MousePress, data.value("onMousePress").toList());
    }

    if (data.contains("onMouseRelease") && data.value("onMouseRelease").type() == QVariant::List) {
        addEventActions(Interaction::MouseRelease, data.value("onMouseRelease").toList());
    }

    if (data.contains("borderWidth") && data.value("borderWidth").canConvert(QVariant::Int))
        setBorderWidth(data.value("borderWidth").toInt());

    if (data.contains("borderColor") && data.value("borderColor").type() == QVariant::List)
        setBorderColor(Utils::listToColor(data.value("borderColor").toList()));

    if (data.contains("keepAspectRatio") && data.value("keepAspectRatio").type() == QVariant::Bool)
        setKeepAspectRatio(data.value("keepAspectRatio").toBool());

    if (data.value("padding").type() == QVariant::Map)
        setPadding(Padding(data.value("padding").toMap()));
}

void Object::filterLoadData(QVariantMap &data)
{
    GameObject::filterLoadData(data);
    data.remove("x");
    data.remove("y");
    data.remove("visible");
}

void Object::onParentResized(int w, int h)
{
    if (mPercentWidth)
        setWidth(width()*w/100);

    if(mPercentHeight)
        setHeight(height()*h/100);
}

int Object::percentWidth() const
{
    return mPercentWidth;
}

int Object::percentHeight() const
{
    return mPercentHeight;
}

int Object::borderWidth()
{
    return mBorderWidth;
}

void Object::setBorderWidth(int w)
{
    if (mBorderWidth == w)
        return;

    mBorderWidth = w;
    this->notify("borderWidth", w);
}

QColor Object::borderColor()
{
    return mBorderColor;
}

void Object::setBorderColor(const QColor & color)
{
    if (mBorderColor == color)
        return;

    mBorderColor = color;
    this->notify("borderColor", Utils::colorToList(mBorderColor));
}

QString Object::defaultFontFamily()
{
    return mDefaultFont.family();
}

void Object::setDefaultFontFamily(const QString& family)
{
    mDefaultFont.setFamily(family);
}

int Object::defaultFontSize()
{
    return mDefaultFont.pixelSize();
}

void Object::setDefaultFontSize(int size)
{
    mDefaultFont.setPixelSize(size);
}

void Object::setDefaultFont(const QFont& font)
{
    mDefaultFont = font;
}

QFont Object::defaultFont()
{
    return mDefaultFont;
}

void Object::show()
{
    if (mBackground.image() && mBackground.image()->movie())
        mBackground.image()->movie()->start();
}

void Object::hide()
{
    if (mBackground.image() && mBackground.image()->movie())
        mBackground.image()->movie()->stop();
}

bool Object::keepAspectRatio()
{
    return mKeepAspectRatio;
}

void Object::setKeepAspectRatio(bool keep)
{
    if (mKeepAspectRatio == keep)
        return;

    mKeepAspectRatio = keep;

    if (mKeepAspectRatio)
        updateAspectRatio();

    if (mKeepAspectRatio && mAspectRatio != 1) {
        int w = width();
        int h = round(w/mAspectRatio);
        mSceneRect.setHeight(h);
        updateResizeRects();
    }

    notify("keepAspectRatio", mKeepAspectRatio);
}

void Object::updateAspectRatio()
{
    mAspectRatio = calculateAspectRatio();
}

float Object::calculateAspectRatio()
{
    return (float) width() / height();
}

void Object::connectToResource()
{
    GameObject::connectToResource();
    Object * resource = qobject_cast<Object*>(this->resource());
    if (resource) {
        connect(resource, SIGNAL(eventActionInserted(Interaction::InputEvent,int,Action*)), this, SLOT(insertEventAction(Interaction::InputEvent,int,Action*)), Qt::UniqueConnection);
        connect(this, SIGNAL(eventActionRemoved(Interaction::InputEvent,Action*,bool)), resource, SLOT(removeEventActionSync(Interaction::InputEvent,Action*,bool)), Qt::UniqueConnection);
        connect(resource, SIGNAL(eventActionRemoved(Interaction::InputEvent,Action*,bool)), this, SLOT(removeEventActionSync(Interaction::InputEvent,Action*,bool)), Qt::UniqueConnection);
        connect(this, SIGNAL(eventActionMoved(Interaction::InputEvent,Action*,int)), resource, SLOT(moveEventActionSync(Interaction::InputEvent,Action*,int)), Qt::UniqueConnection);
        connect(resource, SIGNAL(eventActionMoved(Interaction::InputEvent,Action*,int)), this, SLOT(moveEventActionSync(Interaction::InputEvent,Action*,int)), Qt::UniqueConnection);
        connectEventActions(Interaction::MouseMove, resource);
        connectEventActions(Interaction::MousePress, resource);
        connectEventActions(Interaction::MouseRelease, resource);
    }
}

void Object::connectEventActions(Interaction::InputEvent event, Object * resource)
{
    if (!resource)
        return;

    QList<Action*> resActions, currActions;
    resActions = resource->actionsForEvent(event);
    currActions = actionsForEvent(event);

    for(int i=0; i < resActions.size() && i < currActions.size(); i++) {
        if (!resActions.at(i) || !currActions.at(i))
            break;

        if (resActions.at(i)->type() != currActions.at(i)->type())
            break;

        currActions.at(i)->setResource(resActions.at(i));
        connect(this, SIGNAL(syncChanged(bool)), currActions.at(i), SLOT(setSync(bool)));
    }
}

void Object::setTemporaryBackgroundImage(ImageFile* image)
{
    ImageFile* currImage = mTemporaryBackground.image();
    if (currImage != image) {
        if (currImage && currImage->isAnimated()) {
            AnimatedImage* anim = dynamic_cast<AnimatedImage*>(currImage);
            anim->movie()->stop();
            anim->movie()->disconnect(this);
        }

        if (image && image->isAnimated()) {
            AnimatedImage* anim = dynamic_cast<AnimatedImage*>(image);
            connect(anim->movie(), SIGNAL(frameChanged(int)), this, SIGNAL(dataChanged()));
            anim->movie()->start();
        }

        mTemporaryBackground.setImage(image);
        emit dataChanged();
    }
}

ImageFile* Object::temporaryBackgroundImage() const
{
    return mTemporaryBackground.image();
}

void Object::setTemporaryBackgroundColor(const QColor& color)
{
    if (mTemporaryBackground.color() != color) {
        mTemporaryBackground.setColor(color);
        emit dataChanged();
    }
}

QColor Object::temporaryBackgroundColor() const
{
    return mTemporaryBackground.color();
}

void Object::setTemporaryBackgroundOpacity(int alpha)
{
    if (mTemporaryBackground.opacity() != alpha) {
        mTemporaryBackground.setOpacity(alpha);
        notify("temporaryBackgroundOpacity", mTemporaryBackground.opacity());
    }
}

int Object::temporaryBackgroundOpacity() const
{
    return mTemporaryBackground.opacity();
}

/*void Object::setEditorWidgetFilters(const QStringList& filters)
{
    if (! mEditorWidget)
        return;

    //mEditorWidget->setFilters();
}*/

void Object::alignHorizontally(const QString & alignment)
{
    QString align = alignment.toLower();

    if (align == "left")
        setX(0);
    else if (align == "center")
        setX(Scene::width()/2 - width()/2);
    else if (align == "right")
        setX(Scene::width() - width());
}

void Object::alignVertically(const QString & alignment)
{
    QString align = alignment.toLower();

    if (align == "top")
        setY(0);
    else if (align == "center")
        setY(Scene::height()/2 - height()/2);
    else if (align == "bottom")
        setY(Scene::height() - height());
}

Interaction::InputEvent Object::eventFromSender(QObject* sender)
{
    GameObjectManager* actionManager = qobject_cast<GameObjectManager*>(sender);
    if (!actionManager)
        return Interaction::None;

    QVariant prop = actionManager->property("event");
    if (prop.type() != QVariant::Int)
        return Interaction::None;

    return static_cast<Interaction::InputEvent>(prop.toInt());
}

void Object::onEventActionInserted(int index, GameObject * action)
{
    Interaction::InputEvent event = eventFromSender(sender());
    Action* _action = qobject_cast<Action*>(action);

    if (isSynced() && isResource() && _action) {
        emit eventActionInserted(event, index, _action);
    }
}

void Object::onEventActionRemoved(GameObject * action, bool del)
{
    Interaction::InputEvent event = eventFromSender(sender());
    Action* _action = qobject_cast<Action*>(action);

    bool loadBlocked = blockLoad(true);
    emit eventActionRemoved(event, _action, del);
    blockLoad(loadBlocked);
}

void Object::onEventActionMoved(GameObject* action, int to)
{
    Interaction::InputEvent event = eventFromSender(sender());
    Action* _action = qobject_cast<Action*>(action);

    bool loadBlocked = blockLoad(true);
    emit eventActionMoved(event, _action, to);
    blockLoad(loadBlocked);
}
