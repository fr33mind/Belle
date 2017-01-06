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

#include "action.h"

#include <QDebug>
#include <QVariant>

#include "scene_manager.h"
#include "wait.h"
#include "scene.h"

Action::Action(QObject *parent) :
    GameObject(parent)
{
    init();
}

Action::Action(const QVariantMap& data, QObject *parent) :
    GameObject(data, parent)
{
    init();
    loadInternal(data);

    if (!mTargetParent && !mObject && !mObjectName.isEmpty()) {
        Scene * scene = this->scene();
        if (scene)
            connect(scene, SIGNAL(loaded()), this, SLOT(sceneLoaded()), Qt::UniqueConnection);
    }
}

Action::~Action()
{
}

void Action::init()
{
    setTitle("");
    mObject = 0;
    mObjectName = "";
    mActive = false;
    mAllowSkipping = true;
    mMouseClickOnFinish = false;
    mSupportedEvents = Interaction::None;
    mTargetParent = false;
    mTextEditable = false;
    setType(GameObjectMetaType::Action);
}

void Action::loadData(const QVariantMap & data, bool internal)
{
    if (!internal)
        GameObject::loadData(data, internal);

    if (data.contains("object") && data.value("object").type() == QVariant::String) {
        QString objName = data.value("object").toString();
        Object* obj = 0;
        mTargetParent = false;

        if (data.contains("targetParent") && data.value("targetParent").type() == QVariant::Bool) {
            mTargetParent = data.value("targetParent").toBool();
        }

        if (mTargetParent) {
            obj = parentObject();
        }
        else {
            Scene* scene = this->scene();
            if (scene) {
                obj = scene->object(objName);
            }
        }

        setSceneObject(obj);
        if (!mObject)
            setSceneObjectName(objName);
    }

    if (data.contains("skippable") && data.value("skippable").type() == QVariant::Bool) {
        setAllowSkipping(data.value("skippable").toBool());
    }

    if (data.contains("wait")) {
        setMouseClickOnFinish(true);
    }
    else if (data.contains("waitOnFinished") && data.value("waitOnFinished").type() == QVariant::Bool) {
        setMouseClickOnFinish(data.value("waitOnFinished").toBool());
    }
}

void Action::setTitle(const QString & title)
{
    mTitle = title;
}

QString Action::title() const
{
    if (mTitle.isEmpty()) {
        const GameObjectMetaType* metatype = GameObjectMetaType::metaType(type());
        if (metatype)
            return metatype->name();
    }

    return mTitle;
}

QString Action::description() const
{
    return mDescription;
}

bool Action::allowSkipping() const
{
    return mAllowSkipping;
}

void Action::setAllowSkipping(bool skip)
{
    if (mAllowSkipping == skip)
        return;

    mAllowSkipping = skip;
    notify("skippable", mAllowSkipping);
}

Action* Action::newAction(QObject *parent)
{
    return new Action(parent);
}

void Action::setDescription(const QString& desc)
{
    mDescription = desc;
}

Object* Action::sceneObject() const
{
    return mObject;
}

QString Action::sceneObjectName() const
{
    if (mObject)
        return mObject->name();
    return mObjectName;
}

void Action::setSceneObject(Object * object)
{
    if (mObject == object)
        return;

    removeSceneObject();
    mObject = object;
    mObjectName = mObject ? mObject->name() : "";
    mTargetParent = isParentTargeted();

    connectSceneObject();
    if (isActive())
        loadSceneObject();

    emit sceneObjectChanged(mObject);
    QVariantMap data;
    data.insert("object", mObjectName);
    if (mTargetParent)
        data.insert("targetParent", true);
    notify(data);
}

void Action::setSceneObject(const QString& name)
{
    Scene* scene = this->scene();
    if (scene)
        setSceneObject(scene->object(name));
}

void Action::setSceneObjectName(const QString & name)
{
    if (mObjectName == name)
        return;

    removeSceneObject();
    mObjectName = name;
    notify("object", mObjectName);
}

void Action::onSceneObjectDestroyed()
{
    mObject = 0;
    mObjectName = "";
    emit dataChanged();
}

void Action::paint(const QPainter & painter)
{
}

void Action::setDisplayText(const QString & text)
{
    if (text != mDisplayText) {
        mDisplayText = text;
        emit dataChanged();
    }
}

QString Action::displayText() const
{
    return mDisplayText;
}

QString Action::toString() const
{
    const GameObjectMetaType* metatype = GameObjectMetaType::metaType(type());
    QString typeName = metatype ? metatype->name() : "";
    return QString("%1 [%2]").arg(displayText()).arg(typeName);
}

bool Action::supportsEvent(Interaction::InputEvent ev)
{
    return mSupportedEvents.testFlag(ev);
}

void Action::setSupportedEvents(Interaction::InputEvents events)
{
    mSupportedEvents = events;
}

bool Action::mouseClickOnFinish()
{
    return mMouseClickOnFinish;
}

void Action::setMouseClickOnFinish(bool mouseClick)
{
    if (mMouseClickOnFinish == mouseClick)
        return;

    mMouseClickOnFinish = mouseClick;
    notify("waitOnFinished", mMouseClickOnFinish);
}

void Action::initFrom(Action* action)
{
    mMouseClickOnFinish = action->mouseClickOnFinish();
    mAllowSkipping = action->allowSkipping();
}

QVariantMap Action::toJsonObject(bool internal) const
{
    QVariantMap action = GameObject::toJsonObject(internal);

    if (! mAllowSkipping) //this property is true by default
        action.insert("skippable", mAllowSkipping);
    if (mMouseClickOnFinish) {
        Wait wait;
        wait.setWaitType(Wait::MouseClick);
        action.insert("wait", wait.toJsonObject(internal));
    }

    Object* object = sceneObject();
    if (object) {
        action.insert("object", object->name());
        if (mTargetParent)
            action.insert("targetParent", true);
    }
    else if (! mObjectName.isEmpty())
        action.insert("object", mObjectName);

    return action;
}

void Action::focusIn()
{
    mActive = true;
    loadSceneObject();
}

void Action::focusOut()
{
    restoreSceneObject();
    mActive = false;
}

bool Action::isActive()
{
    return mActive;
}

void Action::sceneLoaded()
{
    if (mObject || mObjectName.isEmpty())
        return;

    Scene* scene = this->scene();
    if (scene) {
        setSceneObject(scene->object(mObjectName));
    }
}

void Action::loadSceneObject()
{
}

void Action::restoreSceneObject()
{
}

void Action::connectSceneObject()
{
    if (mObject) {
        mObjectName = mObject->name();
        connect(mObject, SIGNAL(destroyed()), this, SLOT(onSceneObjectDestroyed()), Qt::UniqueConnection);
    }
}

void Action::disconnectSceneObject()
{
    if (mObject)
        mObject->disconnect(this);
}

void Action::connectToResource()
{
    GameObject::connectToResource();
    Action* resource =  qobject_cast<Action*>(this->resource());
    if (!resource)
        return;

    //TODO: Add an option to check if we should connect names aswell
    connect(this, SIGNAL(nameChanged(const QString&)), resource, SLOT(setName(const QString&)));
    connect(resource, SIGNAL(nameChanged(const QString&)), this, SLOT(setName(const QString&)));

    if (!mObject && resource->isParentTargeted()) {
        setSceneObject(parentObject());
    }
}

Object* Action::parentObject() const
{
    Object* parent = 0;
    QObject* obj = this->parent();

    while(obj) {
        parent = qobject_cast<Object*>(obj);
        if (parent)
            break;
        obj = obj->parent();
    }

    return parent;
}

bool Action::isParentTargeted() const
{
    Object* sceneObj = sceneObject();
    if (!sceneObj)
        return false;

    return parentObject() == sceneObj;
}

void Action::removeSceneObject()
{
    if (isActive())
        restoreSceneObject();
    disconnectSceneObject();
    mObject = 0;
    mObjectName = "";
    mTargetParent = false;
}

bool Action::isTextEditable() const
{
    return mTextEditable;
}

void Action::setTextEditable(bool editable)
{
    mTextEditable = editable;
}

QString Action::editText() const
{
    return "";
}

void Action::setEditText(const QString& text)
{
}
