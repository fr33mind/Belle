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

#include <QDebug>

static ObjectGroupEditorWidget * mEditorWidget = 0;

ObjectGroup::ObjectGroup(QObject *parent,  const QString& name) :
    Object(parent, name)
{
    init();
}

ObjectGroup::ObjectGroup(const QVariantMap& data, QObject *parent) :
    Object(data, parent)
{
    init();
    this->_load(data);
}

void ObjectGroup::_load(const QVariantMap& data)
{
    this->blockNotifications(true);
    ResourceManager * resourceManager = ResourceManager::instance();

    if (data.contains("objects") && data.value("objects").type() == QVariant::List) {
        this->removeAllObjects(true);
        QVariantList objects = data.value("objects").toList();
        Object *obj = 0;

        for(int i=0; i < objects.size(); i++) {
            if (objects[i].type() == QVariant::Map) {
                obj = resourceManager->createObject(objects[i].toMap(), this);
            }
            else if (objects[i].type() == QMetaType::QObjectStar) {
                obj = qobject_cast<Object*>(objects[i].value<QObject*>());
            }

            if(obj)
                _append(obj);
        }
    }

    //internal data passed between resource and clones
    if (data.contains("_object") && data.value("_object").type() == QVariant::Map) {
        QVariantMap object = data.value("_object").toMap();
        if (object.contains("_index") && object.value("_index").canConvert(QVariant::Int)) {
            int _index = object.value("_index").toInt();
            if (_index >= 0 && _index < mObjects.size()) {
                mObjects[_index]->load(object);
                //some properties are ignored by default so we need to apply them here for the child objects
                if (object.contains("visible") && object.value("visible").type() == QVariant::Bool)
                    mObjects[_index]->setVisible(object.value("visible").toBool());
            }
        }
    }

    this->blockNotifications(false);
}

void ObjectGroup::load(const QVariantMap& data)
{
    Object::load(data);
    this->_load(data);
}

void ObjectGroup::init()
{
    setType("ObjectGroup");
    mEditingMode = false;
    mSelectedObject = 0;
}

ObjectGroupEditorWidget* ObjectGroup::objectGroupEditorWidget()
{
    return mEditorWidget;
}

void ObjectGroup::setObjectGroupEditorWidget(ObjectGroupEditorWidget* widget)
{
    mEditorWidget = widget;
}

ObjectEditorWidget* ObjectGroup::editorWidget()
{
    return mEditorWidget;
}

void ObjectGroup::_append(Object* obj)
{
    connect(obj, SIGNAL(dataChanged(const QVariantMap&)), this, SLOT(objectChanged(const QVariantMap&)));
    connect(obj, SIGNAL(destroyed(Object*)), this, SLOT(objectDestroyed(Object*)));
    mObjects.append(obj);
    adaptSize();
    checkStickyObjects();
}

void ObjectGroup::append(Object* obj, int spacing)
{
    if (! obj)
        return;

    int starty = this->y();
    for(int i=0; i < mObjects.size(); i++) {
        if (mObjects[i]->y() + mObjects[i]->height() > starty)
            starty = mObjects[i]->y() + mObjects[i]->height();
    }

    starty += spacing;
    obj->setY(starty);
    obj->setX(this->x());
    _append(obj);
    this->notify("objects", variantObjects());
    //connect(this, SIGNAL(resized(int,int)), obj, SLOT(onParentResized(int, int)));
}

void ObjectGroup::resize(int x, int y)
{
    int prevWidth = this->width();
    int prevHeight = this->height();
    int prevY = this->y();

    Object::resize(x, y);
    if (this->height() < minHeight()) {
        this->setY(prevY);
        this->setHeight(minHeight());
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
    alignObjectsHorizontally();
    alignObjectsVertically();
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
    if (spacing) {
        int starty = this->y();
        for(int i=0; i < mObjects.size(); i++) {
            mObjects[i]->setY(starty);
            starty += mObjects[i]->height() + spacing;
        }
    }
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

    w = right - left;
    h = bottom - top;

    Object::setX(left);
    Object::setY(top);
    Object::setWidth(w);
    Object::setHeight(h);
}

Object * ObjectGroup::object(int index)
{
    if (index >= 0 && index < mObjects.size())
        return mObjects[index];

    return 0;
}

Object * ObjectGroup::object(const QString& name)
{
    for(int i=0; i < mObjects.size(); i++)
        if (mObjects[i]->objectName() == name)
            return mObjects[i];

    return 0;
}

Object * ObjectGroup::object(const QPoint& point)
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

QList<Object*> ObjectGroup::objects()
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
    for(int i=mObjects.size()-1; i >=0; --i) {
        mObjects[i]->setX(x);
    }

    Object::setX(x);
}

void ObjectGroup::setY(int y)
{
    int h = 0;
    for(int i=0; i < mObjects.size(); i++) {
        mObjects[i]->setY(y+h);
        h += mObjects[i]->height();
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

void ObjectGroup::setOpacity(int opacity)
{
    Object::setOpacity(opacity);

    for(int i=0; i < mObjects.size(); i++)
        mObjects[i]->setOpacity(opacity);
}

bool ObjectGroup::editingMode()
{
    return mEditingMode;
}

void ObjectGroup::setEditingMode(bool mode)
{
    mEditingMode = mode;
    if (! mEditingMode) {
        adaptSize();
        checkStickyObjects();
    }
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
    for(int i=0; i < mObjects.size(); i++) {
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

void ObjectGroup::objectChanged(const QVariantMap& data)
{
    Object * sender = qobject_cast<Object*>(this->sender());
    QVariantMap object = data;
    int index = this->indexOf(sender);

    if (! object.isEmpty() && index != -1) {
        object.insert("_index", index);
        this->notify("_object", object);
    }
    else {
        emit dataChanged();
    }
}

QVariantMap ObjectGroup::toJsonObject(bool internal)
{
    QVariantMap object = Object::toJsonObject(internal);
    QVariantList objects;

    for(int i=0; i < mObjects.size(); i++) {
        objects.append(mObjects[i]->toJsonObject(internal));
    }

    if (! objects.isEmpty())
        object.insert("objects", objects);

    return object;
}

