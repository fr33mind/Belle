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

ActionInfo Action::Info;

Action::Action(QObject *parent) :
    GameObject(parent)
{
    init();
}

Action::Action(const QVariantMap& data, QObject *parent) :
    GameObject(data, parent)
{
    init();
    if (data.contains("object") && data.value("object").type() == QVariant::String) {
        mObjectName = data.value("object").toString();

        //check if parent is the target object
        Object* obj = qobject_cast<Object*>(parent);
        if (obj && obj->name() == mObjectName) {
            mObject = obj;
        }

        if (! mObject) {
            Scene * scene = this->scene();
            if (scene)
                connect(scene, SIGNAL(loaded()), this, SLOT(sceneLoaded()));
        }
    }

    if (data.contains("skippable") && data.value("skippable").type() == QVariant::Bool) {
        mAllowSkipping = data.value("skippable").toBool();
    }

    setMouseClickOnFinish(data.contains("wait"));
}

void Action::init()
{
    mObject = 0;
    mObjectName = "";
    mActive = false;
    mAllowSkipping = true;
    mMouseClickOnFinish = false;
    mSupportedEvents = Interaction::None;
    setType(Info.type);
    setTypeName(Info.typeName);
    setIcon(Info.icon);
}

QIcon Action::icon() const
{
    return mIcon;
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
    mAllowSkipping = skip;
}

Action* Action::newAction(QObject *parent)
{
    return new Action(parent);
}

void Action::setIcon(const QIcon & icon)
{
    mIcon = icon;
}

void Action::setTypeName(const QString & typeName)
{
    mTypeName = typeName;
}

QString Action::typeName() const
{
    return mTypeName;
}

void Action::setDescription(const QString& desc)
{
    mDescription = desc;
}

Object* Action::sceneObject() const
{
    if (mObject)
        return mObject;

    if (! mObjectName.isEmpty()) {
        Scene * scene = this->scene();
        if (scene)
          return scene->object(mObjectName);
    }

    return 0;
}

void Action::setSceneObject(Object * object)
{
    if (mObject == object)
        return;

    if (mObject)
        mObject->disconnect(this);

    mObject = object;
    if (mObject) {
        mObjectName = mObject->name();
        connect(mObject, SIGNAL(destroyed()), this, SLOT(onSceneObjectDestroyed()));
        emit dataChanged();
    }
}

void Action::setSceneObject(const QString& name)
{
    Scene* scene = this->scene();
    if (scene)
        setSceneObject(scene->object(name));
}

void Action::onSceneObjectDestroyed()
{
    mObject = 0;
    mObjectName = "";
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
    return QString("%1 [%2]").arg(displayText()).arg(typeName());
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
    mMouseClickOnFinish = mouseClick;
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
    if (object)
        action.insert("object", object->name());
    else if (! mObjectName.isEmpty())
        action.insert("object", mObjectName);

    return action;
}

void Action::focusIn()
{
    mActive = true;
}

void Action::focusOut()
{
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
