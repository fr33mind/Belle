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

#include "clipboard.h"

#include <QDebug>

Clipboard::Clipboard(QObject *parent) :
    QObject(parent)
{
}

bool Clipboard::hasObjects()
{
    return ! objects().isEmpty();
}

bool Clipboard::hasScenes()
{
    return ! scenes().isEmpty();
}

bool Clipboard::hasActions()
{
    return ! actions().isEmpty();
}

QList<Object*> Clipboard::objects()
{
    QList<Object*> objects;
    foreach(QObject* obj, mQObjects) {
        Object* object = qobject_cast<Object*>(obj);
        if (object)
            objects.append(object);
    }

    return objects;
}

QList<Action*> Clipboard::actions()
{
    QList<Action*> actions;
    foreach(QObject* obj, mQObjects) {
        Action* action = qobject_cast<Action*>(obj);
        if (action)
            actions.append(action);
    }

    return actions;
}

QList<Scene*> Clipboard::scenes()
{
    QList<Scene*> scenes;
    foreach(QObject* obj, mQObjects) {
        Scene* scene = qobject_cast<Scene*>(obj);
        if (scene)
            scenes.append(scene);
    }

    return scenes;
}

Clipboard::Operation Clipboard::operation()
{
    return mOperation;
}

void Clipboard::add(QObject * obj, Clipboard::Operation op)
{
    add(QList<QObject*>()<< obj, op);
}

void Clipboard::add(const QList<QObject*>& objects, Clipboard::Operation op)
{
    this->clear();
    mOperation = op;

    foreach(QObject* obj, objects) {
        connect(obj, SIGNAL(destroyed(QObject*)), this, SLOT(onObjectDestroyed(QObject*)));
        mQObjects.append(obj);
    }
}

bool Clipboard::isEmpty()
{
    return mQObjects.isEmpty();
}

void Clipboard::clear()
{
    foreach(QObject*obj, mQObjects)
        obj->disconnect(this);

    mQObjects.clear();
}

void Clipboard::onObjectDestroyed(QObject* object)
{
    if (mQObjects.contains(object))
        mQObjects.removeOne(object);
}
