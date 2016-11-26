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

#include "change_visibility.h"
#include "show.h"
#include "gameobjectfactory.h"

ChangeVisibility::ChangeVisibility(bool show, QObject *parent) :
    Action(parent)
{
    init(show);
}

ChangeVisibility::ChangeVisibility(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    QString type("Show");
    if (data.contains("type") && data.value("type").type() == QVariant::String)
        type = data.value("type").toString();

    if (type == "Hide")
        init(false);
    else
        init(true);

    loadInternal(data);
}

void ChangeVisibility::init(bool show)
{
    mToShow = show;
    setType(mToShow ? GameObjectMetaType::Show : GameObjectMetaType::Hide);
    mFadeAction = 0;
    mSlideAction = 0;
    setDescription(name() + "...");
    setSupportedEvents(Interaction::MousePress | Interaction::MouseRelease |
    Interaction::MouseMove);
}

void ChangeVisibility::loadData(const QVariantMap & data, bool internal)
{
    if (!internal)
        Action::loadData(data, internal);

    if (data.contains("transitions") && data.value("transitions").type() == QVariant::List ) {
        QVariantList transitions = data.value("transitions").toList();
        for(int i=0; i <transitions.size(); i++ ) {
            if (transitions[i].type() != QVariant::Map)
                continue;
            QVariantMap data = transitions[i].toMap();
            if (data.contains("type") && data.value("type") == "Fade")
                mFadeAction = qobject_cast<Fade*>(GameObjectFactory::createAction(data, this));
            if (data.contains("type") && data.value("type") == "Slide")
                mSlideAction = qobject_cast<Slide*>(GameObjectFactory::createAction(data, this));
        }
    }

    if (data.contains("_enableFadeAction") && data.value("_enableFadeAction").type() == QMetaType::QObjectStar) {
        setFadeActionEnabled(true);
        Fade* action = data.value("_enableFadeAction").value<Fade*>();
        if (mFadeAction && action) {
            if (action->isResource())
                mFadeAction->setResource(action);
            else if (isResource())
                action->setResource(mFadeAction);
        }
    }

    if (data.contains("_disableFadeAction")) {
        setFadeActionEnabled(false);
    }

    if (data.contains("_enableSlideAction") && data.value("_enableSlideAction").type() == QMetaType::QObjectStar) {
        setSlideActionEnabled(true);
        Slide* action = data.value("_enableSlideAction").value<Slide*>();
        if (mSlideAction && action) {
            if (action->isResource())
                mSlideAction->setResource(action);
            else if (isResource())
                action->setResource(mSlideAction);
        }
    }

    if (data.contains("_disableSlideAction")) {
        setSlideActionEnabled(false);
    }
}

void ChangeVisibility::connectToResource()
{
    Action::connectToResource();
    ChangeVisibility* resource = qobject_cast<ChangeVisibility*>(this->resource());
    if (resource) {
        if (mFadeAction) {
            mFadeAction->setResource(resource->fadeAction());
        }

        if (mSlideAction) {
            mSlideAction->setResource(resource->slideAction());
        }
    }
}

QString ChangeVisibility::displayText() const
{
    if (! sceneObject())
        return "";

    QString text("");
    if (sceneObject())
        text += sceneObject()->objectName() + " ";

    Character* character = qobject_cast<Character*>(sceneObject());
    if (character && toShow()) {
        const Show* _this = qobject_cast<const Show*>(this);
        if (_this) {
            text += QString("(%1) ").arg(_this->characterState());
        }
    }

    const Fade* fade = fadeAction();
    const Slide* slide = slideAction();

    double duration = 0.0;

    if (fade && fade->duration() || slide && slide->duration()) {
        text += tr("with") + " ";

        if (fade && fade->duration()) {
            text += QString("%1%2").arg("fade").arg(fade->fadeTypeString());
            if (fade->duration() > duration)
                duration = fade->duration();
        }

        if (fade && fade->duration() && slide && slide->duration())
            text += " " + tr("and") + " ";

        if (slide && slide->duration()) {
            text += tr("slide");
            if (slide->duration() > duration)
                duration = slide->duration();
        }
        text += QString(" %1 %2s").arg(tr("in")).arg(QString::number(duration,'f', 2));
    }

    return text;
}

void ChangeVisibility::setSceneObject(Object* obj)
{
    Action::setSceneObject(obj);

    if (mFadeAction)
        mFadeAction->setSceneObject(obj);
    if (mSlideAction)
        mSlideAction->setSceneObject(obj);

    Character* character = qobject_cast<Character*>(obj);
    if (character)
        setDisplayText(obj->objectName() + " (" + character->currentState() + ")");
    else
        setDisplayText(obj->objectName());
}

bool ChangeVisibility::toShow() const
{
    return mToShow;
}

bool ChangeVisibility::toHide() const
{
    return ! mToShow;
}

void ChangeVisibility::setFadeActionEnabled(bool enable)
{
    if (enable) {
        if (!mFadeAction) {
            Action* fadeAction = GameObjectFactory::createAction(GameObjectMetaType::Fade, this);
            mFadeAction = static_cast<Fade*>(fadeAction);
            mFadeAction->setDuration(0);
            connect(mFadeAction, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()), Qt::UniqueConnection);
            if (mToShow) {
                mFadeAction->setFadeType(Fade::In);
            }
            else {
                mFadeAction->setFadeType(Fade::Out);
            }

            notify("_enableFadeAction", QVariant::fromValue(qobject_cast<QObject*>(mFadeAction)));
        }
    }
    else if (mFadeAction) {
        delete mFadeAction;
        mFadeAction = 0;
        notify("_disableFadeAction", true);
    }
}

bool ChangeVisibility::isFadeActionEnabled() const
{
    return mFadeAction ? true: false;
}

void ChangeVisibility::setFadeAction(Fade* action)
{
    mFadeAction = action;
}

Fade* ChangeVisibility::fadeAction() const
{
    return mFadeAction;
}

void ChangeVisibility::setSlideActionEnabled(bool enable)
{
    if (enable) {
        if (! mSlideAction) {
            Action* slideAction = GameObjectFactory::createAction(GameObjectMetaType::Slide, this);
            mSlideAction = static_cast<Slide*>(slideAction);
            mSlideAction->setDuration(0);
            connect(mSlideAction, SIGNAL(dataChanged()), this, SIGNAL(dataChanged()), Qt::UniqueConnection);
            notify("_enableSlideAction", QVariant::fromValue(qobject_cast<QObject*>(mSlideAction)));
        }
    }
    else if (mSlideAction) {
        mSlideAction->deleteLater();
        mSlideAction = 0;
        notify("_disableSlideAction", true);
    }
}

bool ChangeVisibility::isSlideActionEnabled() const
{
    return mSlideAction ? true : false;
}

void ChangeVisibility::setSlideAction(Slide* action)
{
    mSlideAction = action;
}

Slide* ChangeVisibility::slideAction() const
{
    return mSlideAction;
}

QVariantMap ChangeVisibility::toJsonObject(bool internal) const
{
    QVariantMap object = Action::toJsonObject(internal);
    QVariantList transitions;
    if (mSlideAction)
        transitions.append(mSlideAction->toJsonObject(internal));
    if (mFadeAction)
        transitions.append(mFadeAction->toJsonObject(internal));
    object.insert("transitions", transitions);

    return object;
}

void ChangeVisibility::setToShow(bool show)
{
    mToShow = show;
}
