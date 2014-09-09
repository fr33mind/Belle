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
    ResourceManager * resourceManager = ResourceManager::instance();

    if (data.contains("objects") && data.value("objects").type() == QVariant::List) {
        QVariantList objects = data.value("objects").toList();
        Object *obj = 0;

        for(int i=0; i < objects.size(); i++) {
            if (objects[i].type() == QVariant::Map) {
                obj = resourceManager->createResource(objects[i].toMap(), false, this);
                if(obj)
                    append(obj);
            }
            else
                qWarning() << Q_FUNC_INFO << "Couldn't create resource because it's not a valid JSON object.";
        }
    }
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
    obj->setX(x());
    connect(obj, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()));
    mObjects.append(obj);
    resize();
    alignObjects();
    //connect(this, SIGNAL(resized(int,int)), obj, SLOT(onParentResized(int, int)));
}

void ObjectGroup::resize(int x, int y)
{
    Object::resize(x, y);

    for(int i=0; i < mObjects.size(); i++) {
        mObjects[i]->setX(this->x());
        mObjects[i]->setWidth(this->width());
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

void ObjectGroup::alignObjects()
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

void ObjectGroup::resize()
{
    int h = 0;
    int w = width();

    if (mObjects.size()) {
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
    }

    setWidth(w);
    setHeight(h);
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

Object* ObjectGroup::objectAt(qreal x, qreal y)
{
    if (mEditingMode) {
        for(int i=mObjects.size()-1; i >= 0; i--)
            if (mObjects[i]->contains(x, y)) {
                return mObjects[i];
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

bool ObjectGroup::editingMode()
{
    return mEditingMode;
}

void ObjectGroup::setEditingMode(bool mode)
{
    mEditingMode = mode;
    if (! mEditingMode) {
        resize();
    }
}

Object* ObjectGroup::removeObjectAt(int index)
{
    if (index >= 0 && index < mObjects.size()) {
        Object* obj = mObjects.takeAt(index);
        obj->disconnect(this);
        resize();
        return obj;
    }
    return 0;
}

void ObjectGroup::deleteObjectAt(int index)
{
    Object* obj = removeObjectAt(index);
    if (obj)
        obj->deleteLater();
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

