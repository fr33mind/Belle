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

#include "resource_manager.h"

#include <QDebug>
#include <QMovie>

#include "utils.h"
#include "image.h"
#include "character.h"
#include "dialoguebox.h"
#include "textbox.h"
#include "button.h"
#include "menu.h"
#include "animatedimage.h"

static QList<Object*> mResources;
static ResourceManager* mInstance = new ResourceManager();

ResourceManager::ResourceManager(QObject *parent) :
    QObject(parent)
{
}

void ResourceManager::addResource(Object * obj)
{
    if (obj) {
        if (! isValidName(obj->objectName()))
           obj->setObjectName(newName(obj));

        if (! obj->parent())
            obj->setParent(this);

        connect(obj, SIGNAL(dataChanged()), this, SIGNAL(resourceChanged()));
        mResources.append(obj);
        emit resourceAdded(obj);
    }
}

bool ResourceManager::isValidName(const QString& name)
{
    if (name.isEmpty() || name.isNull())
        return false;

    for(int i=0; i < mResources.size(); i++)
        if (mResources[i]->objectName() == name)
            return false;

    return true;
}

QString ResourceManager::newName(QString name)
{
    if (name.isEmpty() || name.isNull())
        name = "object";

    while(! isValidName(name)) {
        name = Utils::incrementLastNumber(name);
    }

    return name;
}

QString ResourceManager::newName(Object* obj)
{
   QString name = obj->objectName();
   if (name.isEmpty() || name.isNull())
        name = obj->type();

   return newName(name);
}

QList<Object*> ResourceManager::resources()
{
    return mResources;
}

Object * ResourceManager::typeToObject(const QString& type, QVariantMap& data, QObject* parent)
{
    QString _type = type.toLower();

    if (_type == "object")
        return new Object(data, parent);
    else if (_type == "image")
        return new Image(data, parent);
    else if (_type == "character")
        return new Character(data, parent);
    else if (_type == "dialoguebox")
        return new DialogueBox(data, parent);
    else if (_type == "textbox")
        return new TextBox(data, parent);
    else if (_type == "button")
        return new Button(data, parent);
    else if (_type == "objectgroup")
        return new ObjectGroup(data, parent);
    else if (_type == "menu")
        return new Menu(data, parent);
    return 0;

}

Object* ResourceManager::createResource(QVariantMap data, bool appendToResources, QObject* parent)
{
    Object* _resource = 0;
    QString type("");

    fillWithResourceData(data);

    if (data.contains("resource") && data.value("resource").type() == QVariant::String)
        _resource = resource(data.value("resource").toString());

    if (data.contains("type") && data.value("type").type() == QVariant::String)
        type = data.value("type").toString();

    Object *object = typeToObject(type, data, parent);
    if (object && _resource)
        object->setResource(_resource);

    if (appendToResources)
        addResource(object);
    return object;
}

void ResourceManager::removeResource(Object *object, bool del)
{
    if (mResources.contains(object)) {
        mResources.removeOne(object);
        object->disconnect(this);
        emit resourceRemoved(object);
        if (del && object)
            object->deleteLater();
    }
}

void ResourceManager::fillWithResourceData(QVariantMap& data)
{
    if (! data.contains("resource") || data.value("resource").type() != QVariant::String)
        return;

    QString name = data.value("resource").toString();

    Object* resource = ResourceManager::resource(name);
    if (! resource)
        return;

    QVariantMap resourceData = resource->toJsonObject();
    QStringList keys = resourceData.keys();
    foreach(const QString& key, keys) {
        if (! data.contains(key))
            data.insert(key, resourceData.value(key));
    }
}

Object *ResourceManager::resource(const QString & name)
{
    for(int i=0; i < mResources.size(); i++) {
        if (mResources[i]->objectName() == name)
            return mResources[i];
    }

    return 0;
}

Object *ResourceManager::resource(int index)
{
    if (index >= mResources.size() || index < 0)
        return 0;

    return mResources.at(index);
}


bool ResourceManager::contains(const QString & name)
{
    if ( resource(name) )
        return true;
    return false;
}

void  ResourceManager::removeResources(bool del)
{
    for(int i=mResources.size()-1; i >= 0; i--)
        removeResource(mResources[i], del);

    mResources.clear();
}

void  ResourceManager::destroy()
{
    mInstance->removeResources(true);
    mInstance->deleteLater();
}

ResourceManager* ResourceManager::instance()
{
    return mInstance;
}

void ResourceManager::importResources(const QVariantMap& data)
{
    if (data.contains("resources") && data.value("resources").type() == QVariant::Map) {
        QVariantMap resourcesMap = data.value("resources").toMap();
        QMapIterator<QString, QVariant> it(resourcesMap);
        while(it.hasNext()) {
            it.next();
            if (it.value().type() != QVariant::Map)
                continue;

            ResourceManager::instance()->createResource(it.value().toMap());
        }
    }
}

QVariantMap ResourceManager::exportResources()
{
    QVariantMap resourcesData;
    QList<Object*> resources = ResourceManager::resources();
    for (int i=0; i < resources.size(); i++) {
        resourcesData.insert(resources.at(i)->objectName(), resources.at(i)->toJsonObject(false));
    }
    return resourcesData;
}

