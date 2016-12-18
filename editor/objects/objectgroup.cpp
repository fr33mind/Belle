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

#include "objectgroup.h"

ObjectGroup::ObjectGroup(QObject *parent,  const QString& name) :
    Object(parent, name)
{
    init();
}

ObjectGroup::ObjectGroup(const QVariantMap& data, QObject *parent) :
    Object(data, parent)
{
    init();
    loadInternal(data);
}

void ObjectGroup::loadData(const QVariantMap& data, bool internal)
{
    if (!internal)
        Object::loadData(data, internal);

    Object* obj = 0;

    if (data.contains("objects") && data.value("objects").type() == QVariant::List) {
        this->removeAllObjects(true);
        QVariantList objects = data.value("objects").toList();
        Object *obj = 0;

        for(int i=0; i < objects.size(); i++) {
            if (objects[i].type() == QVariant::Map) {
                obj = createObject(objects[i].toMap());
            }
            else if (objects[i].type() == QMetaType::QObjectStar) {
                obj = qobject_cast<Object*>(objects[i].value<QObject*>());
            }

            if(obj)
                _append(obj);
        }

        checkStickyObjects();
        setSpacing(calcSpacing());
    }

    //internal data passed between resource and clones
    if (data.contains("_object") && data.value("_object").type() == QVariant::Map) {
        QVariantMap object = data.value("_object").toMap();
        if (object.contains("_index") && object.value("_index").canConvert(QVariant::Int)) {
            int _index = object.value("_index").toInt();
            if (_index >= 0 && _index < mObjects.size()) {
                obj = mObjects.at(_index);
                if (obj) {
                    obj->load(object);
                    //some properties are ignored by default so we need to apply them here for the child objects
                    if (object.contains("visible") && object.value("visible").type() == QVariant::Bool)
                        obj->setVisible(object.value("visible").toBool());
                }
            }
        }
    }

    if (data.contains("editingModeData") && data.value("editingModeData").type() == QVariant::Map) {
        QVariantMap editingModeData = data.value("editingModeData").toMap();
        QVariantList rects = editingModeData.value("rects").toList();
        QRect rect;
        setEditingMode(true);
        for(int i=0; i < mObjects.size() && i < rects.size(); i++) {
            obj = mObjects.at(i);
            rect = rects.at(i).toRect();
            if (obj) {
                obj->setX(this->x() + rect.x());
                obj->setY(this->y() + rect.y());
                obj->setWidth(rect.width());
                obj->setHeight(rect.height());
            }
        }
        setEditingMode(false);
    }

    if (data.contains("alignObjects")) {
        alignObjects();
    }

    if (data.contains("spacing") && data.value("spacing").canConvert(QVariant::Int)) {
        bool ok = false;
        int spacing = data.value("spacing").toInt(&ok);
        if (ok)
            setSpacing(spacing);
    }
}

void ObjectGroup::init()
{
    setType(GameObjectMetaType::ObjectGroup);
    mEditingMode = false;
    mSelectedObject = 0;
    mAligning = false;
    mSpacing = 0;
}

void ObjectGroup::_append(Object* obj)
{
    connect(obj, SIGNAL(dataChanged(const QVariantMap&)), this, SLOT(objectChanged(const QVariantMap&)));
    connect(obj, SIGNAL(destroyed(Object*)), this, SLOT(objectDestroyed(Object*)));
    mObjects.append(obj);
}

void ObjectGroup::append(Object* obj)
{
    if (! obj)
        return;

    int starty = this->y();
    for(int i=0; i < mObjects.size(); i++) {
        if (mObjects[i]->y() + mObjects[i]->height() > starty)
            starty = mObjects[i]->y() + mObjects[i]->height();
    }

    if (mObjects.size())
        starty += mSpacing;
    obj->setY(starty);
    obj->setX(this->x());
    _append(obj);
    adaptSize();
    checkStickyObjects();
    this->notify("objects", variantObjects());
    //connect(this, SIGNAL(resized(int,int)), obj, SLOT(onParentResized(int, int)));
}

void ObjectGroup::resize(int x, int y)
{
    int prevY = this->y();
    int minHeight = this->minHeight();

    Object::resize(x, y);
    if (this->height() < minHeight) {
        Object::setY(prevY);
        Object::setHeight(minHeight);
    }

    this->alignObjects();
}

int ObjectGroup::calcSpacing() const
{
    if (mObjects.size() < 2)
        return 0;

    int sumObjHeight = 0;
    int height = this->height();
    for(int i=0; i < mObjects.size(); i++)
        sumObjHeight += mObjects[i]->height();
    int spacing = (height - sumObjHeight) / (mObjects.size()-1);
    if (height > sumObjHeight) {
        return spacing;
    }

    return 0;
}

void ObjectGroup::setSpacing(int spacing)
{
    if (mSpacing == spacing)
        return;

    int y = this->y();

    for(int i=0; i < mObjects.size(); i++) {
        mObjects[i]->setY(y);
        y += mObjects[i]->height() + spacing;
    }

    mSpacing = spacing;
    notify("spacing", mSpacing);
}

int ObjectGroup::spacing() const
{
    return mSpacing;
}

int ObjectGroup::minHeight() const
{
    int height = 0;
    for(int i=0; i < mObjects.size(); i++) {
        height += mObjects[i]->height();
    }
    return height;
}

void ObjectGroup::alignObjects()
{
    mAligning = true;
    alignObjectsHorizontally();
    alignObjectsVertically();
    mAligning = false;
    notify("alignObjects", true);
}

void ObjectGroup::alignObjectsHorizontally()
{
    QRect rect = this->sceneRect(), objRect;
    int x = this->x(), objX=0, leftspace=0, objWidth=0;

    for(int i=0; i < mObjects.size(); i++) {
        objRect = mObjects[i]->sceneRect();
        objWidth = mObjects[i]->width();

        if (objRect.left() < rect.left()) {
            mObjects[i]->setX(x);
            objRect = mObjects[i]->sceneRect();
        }
        else if (rect.left() < objRect.left()) {
            if (mStickyObjects.contains(mObjects[i]))
                mObjects[i]->setWidth(this->width());
        }

        if (objRect.right() > rect.right()) {
            objX = mObjects[i]->x();
            leftspace = objRect.x() - rect.x();
            leftspace -= objRect.right() - rect.right();
            objX = x;
            if (leftspace >= 0)
                objX = x + leftspace;
            else
                objWidth -= leftspace * -1;
            mObjects[i]->setX(objX);
            mObjects[i]->setWidth(objWidth);
            addStickyObject(mObjects[i]);
        }
        else if (objRect.right() < rect.right()) {
            if (mStickyObjects.contains(mObjects[i]))
                mObjects[i]->setWidth(this->width());
        }
    }
}

void ObjectGroup::alignObjectsVertically()
{
    int spacing = this->calcSpacing();
    int starty = this->y();
    for(int i=0; i < mObjects.size(); i++) {
        mObjects[i]->setY(starty);
        starty += mObjects[i]->height() + spacing;
    }

    mSpacing = spacing;
    notify("spacing", spacing);
}

void ObjectGroup::adaptSize()
{
    if (mObjects.isEmpty())
        return;

    int h = 0;
    int w = width();

    QRect rect = mObjects.first()->sceneRect();
    int top = rect.top();
    int left = rect.left();
    int right = rect.right();
    int bottom = rect.bottom();

    for(int i=1; i < mObjects.size(); i++) {
        rect = mObjects[i]->sceneRect();
        if (rect.top() < top)
            top = rect.top();
        if (rect.left() < left)
            left = rect.left();
        if (rect.right() > right)
            right = rect.right();
        if (rect.bottom() > bottom)
            bottom = rect.bottom();
    }

    //Due to Qt's peculiar behaviour we need to add 1 to right/bottom
    w = (right+1) - left;
    h = (bottom+1) - top;

    Object::setX(left);
    Object::setY(top);
    Object::setWidth(w);
    Object::setHeight(h);
}

Object * ObjectGroup::object(int index) const
{
    if (index >= 0 && index < mObjects.size())
        return mObjects[index];

    return 0;
}

Object * ObjectGroup::object(const QString& name) const
{
    for(int i=0; i < mObjects.size(); i++)
        if (mObjects[i]->objectName() == name)
            return mObjects[i];

    return 0;
}

Object * ObjectGroup::object(const QPoint& point) const
{
    for(int i=mObjects.size()-1; i >=0; --i)
        if (mObjects[i]->contains(point.x(), point.y()))
            return mObjects[i];

    return 0;
}

void ObjectGroup::move(int x, int y)
{
    QRect prevSceneRect = sceneRect();
    int difX, difY;
    Object::move(x, y);

    difX = sceneRect().topLeft().x() - prevSceneRect.topLeft().x();
    difY = sceneRect().topLeft().y() - prevSceneRect.topLeft().y();

    for(int i=mObjects.size()-1; i >=0; --i) {
        mObjects[i]->move( mObjects[i]->x()+difX,  mObjects[i]->y()+difY);
    }
}

void ObjectGroup::paint(QPainter & painter)
{
    Object::paint(painter);
    if (! painter.opacity())
        return;

    for(int i=0; i < mObjects.size(); i++)
       mObjects[i]->paint(painter);
}

QList<Object*> ObjectGroup::objects() const
{
    return mObjects;
}

QVariantList ObjectGroup::variantObjects() const
{
    QVariantList objects;
    for(int i=0; i < mObjects.size(); i++) {
        objects.append(QVariant::fromValue(qobject_cast<QObject*>(mObjects[i])));
    }
    return objects;
}

Object* ObjectGroup::objectAt(qreal x, qreal y)
{
    if (mEditingMode) {
        for(int i=mObjects.size()-1; i >= 0; i--) {
            if (mObjects[i]->contains(x, y)) {
                return mObjects[i];
            }
        }
    }
    if (this->contains(x, y))
        return this;
    return 0;
}

void ObjectGroup::setX(int x)
{
    int move = x - this->x();
    //FIXME: Only works for vertical layouts.
    for(int i=mObjects.size()-1; i >=0; --i) {
        mObjects[i]->setX(mObjects[i]->x()+move);
    }

    Object::setX(x);
}

void ObjectGroup::setY(int y)
{
    int move = y - this->y();
    for(int i=0; i < mObjects.size(); i++) {
        mObjects[i]->setY(mObjects[i]->y()+move);
    }

    Object::setY(y);
}

void ObjectGroup::setWidth(int width, bool percent)
{
    Object::setWidth(width, percent);

    /*for(int i=mObjects.size()-1; i >=0; --i) {
        mObjects[i]->setWidth(width, mObjects[i]->percentWidth());
    }*/
}

void ObjectGroup::setHeight(int height, bool percent)
{
    Object::setHeight(height, percent);

    /*for(int i=mObjects.size()-1; i >=0; --i) {
        mObjects[i]->setHeight(height, mObjects[i]->percentHeight());
    }*/
}

bool ObjectGroup::editingMode() const
{
    return mEditingMode;
}

void ObjectGroup::setEditingMode(bool mode)
{
    if (mEditingMode == mode)
        return;

    mEditingMode = mode;
    if (! mEditingMode) {
        adaptSize();
        checkStickyObjects();
        QVariantMap data;
        data.insert("rects", objectsRelativeRectsData());
        notify("editingModeData", data);
    }
}

QVariantList ObjectGroup::objectsRelativeRectsData()
{
    QVariantList data;
    QRect rect;

    foreach(Object* obj, mObjects) {
        rect = obj->sceneRect();
        rect.moveLeft(rect.x() - this->x());
        rect.moveTop(rect.y() - this->y());
        data.append(rect);
    }

    return data;
}

void ObjectGroup::checkStickyObjects()
{
    mStickyObjects.clear();
    for(int i=0; i < mObjects.size(); i++) {
        if (mObjects[i]->width() == this->width())
            mStickyObjects.append(mObjects[i]);
    }
}

void ObjectGroup::addStickyObject(Object * obj)
{
    if (obj->width() == this->width() && ! mStickyObjects.contains(obj)) {
        mStickyObjects.append(obj);
    }
}

void ObjectGroup::removeObjectAt(int index, bool del)
{
    if (index >= 0 && index < mObjects.size()) {
        Object* obj = mObjects.takeAt(index);
        mStickyObjects.removeAt(index);
        this->notify("objects", variantObjects());
        obj->disconnect(this);
        if (del)
            obj->deleteLater();
        adaptSize();
    }
}

void ObjectGroup::removeAllObjects(bool del)
{
    bool signalsAlreadyBlocked = this->signalsBlocked();
    //don't emit a signal for each removed object
    //instead emit a signal with empty list
    if (! signalsAlreadyBlocked)
        this->blockSignals(true);
    for(int i=mObjects.size()-1; i >= 0; i--) {
        this->removeObjectAt(i, del);
    }
    this->blockSignals(signalsAlreadyBlocked);
    this->notify("objects", QVariantList());
}


int ObjectGroup::indexOf(Object * object)
{
    if (object) {
        return mObjects.indexOf(object);
    }

    return -1;
}

void ObjectGroup::objectDestroyed(Object * object)
{
    int index = mObjects.indexOf(object);
    if (index != -1)
        this->removeObjectAt(index);
}

QVariantMap ObjectGroup::prepareObjectData(int index, const QVariantMap & data)
{
    QVariantMap object = data;

    object.insert("_index", index);

    if (object.contains("width"))
        object.remove("width");

    if (object.contains("height"))
        object.remove("height");

    if (object.contains("x"))
        object.remove("x");

    if (object.contains("y"))
        object.remove("y");

    return object;
}

void ObjectGroup::objectChanged(const QVariantMap& data)
{
    Object * sender = qobject_cast<Object*>(this->sender());
    QVariantMap object;
    int index = this->indexOf(sender);

    if (index != -1) {
        object = prepareObjectData(index, data);
        if (!object.isEmpty())
            this->notify("_object", object);
        if (!mAligning) {
            adaptSize();
            checkStickyObjects();
        }
    }
    else {
        emit dataChanged();
    }
}

QVariantMap ObjectGroup::toJsonObject(bool internal) const
{
    QVariantMap object = Object::toJsonObject(internal);
    QVariantList objects;
    QVariantMap objData;
    int x = this->x();
    int y = this->y();

    for(int i=0; i < mObjects.size(); i++) {
        objData = mObjects[i]->toJsonObject(internal);
        if (internal) {
            objData.insert("relativeX", mObjects[i]->x() - x);
            objData.insert("relativeY", mObjects[i]->y() - y);
        }
        objects.append(objData);
    }

    if (! objects.isEmpty())
        object.insert("objects", objects);

    return object;
}

Object* ObjectGroup::createObject(const QVariantMap & data)
{
    Object* obj = ResourceManager::instance()->createObject(data, this);
    int x = this->x();
    int y = this->y();

    if (obj) {
        if (data.contains("relativeX") && data.value("relativeX").canConvert(QVariant::Int))
            obj->setX(x + data.value("relativeX").toInt());

        if (data.contains("relativeY") && data.value("relativeY").canConvert(QVariant::Int))
            obj->setY(y + data.value("relativeY").toInt());
    }

    return obj;
}

void ObjectGroup::updateSpacing()
{
    mSpacing = calcSpacing();
}

void ObjectGroup::adaptLayout()
{
    adaptSize();
    updateSpacing();
    checkStickyObjects();
}
