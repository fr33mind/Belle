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

static ActionEditorWidget* mEditorWidget = 0;
ActionInfo Action::Info;

Action::Action(QObject *parent) :
    QObject(parent)
{
    init();
}

Action::Action(const QVariantMap& data, QObject *parent) :
    QObject(parent)
{
    init();

    if (parent && data.contains("object") && data.value("object").type() == QVariant::String) {
        Scene* scene = qobject_cast<Scene*>(parent);
        if (scene) {
            Object* obj = scene->object(data.value("object").toString());
            if (obj)
                mObject = obj;
        }
        else {
            //This action maybe be inside another action
            Action* action = qobject_cast<Action*>(parent);
            if (action)
                mObject = action->sceneObject();
        }
    }

    if (data.contains("name") && data.value("name").type() == QVariant::String) {
        setObjectName(data.value("name").toString());
    }

    if (data.contains("skippable") && data.value("skippable").type() == QVariant::Bool) {
        mAllowSkipping = data.value("skippable").toBool();
    }

    if (data.contains("type") && data.value("type").type() == QVariant::String) {
        setType(data.value("type").toString());
    }

    setMouseClickOnFinish(data.contains("wait"));
}

void Action::init()
{
    mObject = 0;
    mActive = false;
    mAllowSkipping = true;
    mMouseClickOnFinish = false;
    mSupportedEvents = Interaction::None;
    setType(Info.type);
    setName(Info.name);
    setIcon(Info.icon);
    connect(this, SIGNAL(dataChanged()), this, SLOT(onDataChanged()));
}


void Action::setActionEditorWidget(ActionEditorWidget* widget)
{
    mEditorWidget = widget;
}

ActionEditorWidget* Action::actionEditorWidget()
{
    return mEditorWidget;
}

ActionEditorWidget* Action::editorWidget()
{
    return mEditorWidget;
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

void Action::setDescription(const QString& desc)
{
    mDescription = desc;
}

QString Action::name() const
{
    return mName;
}

void Action::setName(const QString & name)
{
    mName = name;
}


Object* Action::sceneObject() const
{
    return mObject;
}

void Action::setSceneObject(Object * object)
{
    if (mObject)
        mObject->disconnect(this);
    mObject = object;
    if (mObject)
        connect(mObject, SIGNAL(destroyed()), this, SLOT(onSceneObjectDestroyed()));
}

void Action::onSceneObjectDestroyed()
{
    mObject = 0;
}

void Action::onDataChanged()
{
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
    return QString("%1 [%2]").arg(displayText()).arg(mName);
}

QString Action::type() const
{
    return mType;
}

void Action::setType(const QString & type)
{
    mType = type;
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

QVariantMap Action::toJsonObject()
{
    QVariantMap action;
    if (! objectName().isEmpty())
        action.insert("name", objectName());

    action.insert("type", mType);
    if (! mAllowSkipping) //this property is true by default
        action.insert("skippable", mAllowSkipping);
    if (mMouseClickOnFinish) {
        Wait wait;
        wait.setWaitType(Wait::MouseClick);
        action.insert("wait", wait.toJsonObject());
    }

    if (mObject)
        action.insert("object", mObject->objectName());

    return action;
}

void Action::setObjectName(const QString &name)
{
    QObject::setObjectName(name);

    emit objectNameChanged();
}

void Action::focusIn()
{
    mActive = true;
}

void Action::focusOut()
{
    mActive = false;
}

Scene* Action::scene()
{
    //shouldn't happen, but just in case
    if (! this->parent())
        return 0;

    //usual case - action's parent is the scene
    if (qobject_cast<Scene*>(this->parent()))
        return qobject_cast<Scene*>(this->parent());

    //in case this action is inside another action
    if (qobject_cast<Action*>(this->parent())) {
        Action* action = qobject_cast<Action*>(this->parent());
        if (action->scene())
            return action->scene();
    }

    //shouldn't happen either, but just in case
    return 0;
}

bool Action::isActive()
{
    return mActive;
}
