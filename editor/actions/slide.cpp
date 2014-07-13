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

#include "slide.h"

#include <QDebug>

static SlideEditorWidget* mEditorWidget = 0;
ActionInfo Slide::Info;

Slide::Slide(QObject *parent) :
    Action(parent)
{
    init();
}

Slide::Slide(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();

    if (data.contains("startX") && data.value("startX").canConvert(QVariant::Double))
        setStartX(data.value("startX").toFloat());

    if (data.contains("startY") && data.value("startY").canConvert(QVariant::Double))
        setStartY(data.value("startY").toFloat());

    if (data.contains("endX") && data.value("endX").canConvert(QVariant::Double))
        setEndX(data.value("endX").toFloat());

    if (data.contains("endY") && data.value("endY").canConvert(QVariant::Double))
        setEndY(data.value("endY").toFloat());

    if (data.contains("duration") && data.value("duration").canConvert(QVariant::Double))
        setDuration(data.value("duration").toFloat());
}


void Slide::init()
{
    setType(Info.type);
    setName(Info.name);
    setIcon(Info.icon);
    setSupportedEvents(Interaction::MousePress | Interaction::MouseRelease |
    Interaction::MouseMove);
    mObjectOriginalPoint = QPoint();
    mDuration = 1;
}

void Slide::setSlideEditorWidget(SlideEditorWidget * widget)
{
   mEditorWidget = widget;
}

SlideEditorWidget* Slide::slideEditorWidget()
{
   return mEditorWidget;
}

ActionEditorWidget* Slide::editorWidget()
{
    return mEditorWidget;
}

QString Slide::displayText() const
{
    if (! this->sceneObject())
        return "";

    return QString("\"%1\" from (%2, %3) to (%4, %5)").arg(this->sceneObject()->objectName())
                                                .arg(mStartPoint.x())
                                                .arg(mStartPoint.y())
                                                .arg(mEndPoint.x())
                                                .arg(mEndPoint.y());
}

int Slide::startX() const
{
    return mStartPoint.x();
}

int Slide::startY() const
{
    return mStartPoint.y();
}

int Slide::endX() const
{
    return mEndPoint.x();
}

int Slide::endY() const
{
    return mEndPoint.y();
}

void Slide::setStartPoint(const QPoint& point)
{
    mStartPoint = point;
}

QPoint Slide::startPoint() const
{
    return mStartPoint;
}

void Slide::setStartX(int x)
{
    mStartPoint.setX(x);
    //if (sceneObject())
    //    sceneObject()->setX(x);
    emit dataChanged();
}

void Slide::setStartY(int y)
{
    mStartPoint.setY(y);
    //if (sceneObject())
    //    sceneObject()->setY(y);
    emit dataChanged();
}

QPoint Slide::endPoint() const
{
    return mEndPoint;
}

void Slide::setEndPoint(const QPoint& point)
{
    mEndPoint = point;
}

void Slide::setEndX(int x)
{
    mEndPoint.setX(x);
    //if (sceneObject())
    //    sceneObject()->setX(x);
    emit dataChanged();
}

void Slide::setEndY(int y)
{
    mEndPoint.setY(y);
    //if (sceneObject())
     //   sceneObject()->setY(y);
    emit dataChanged();
}

/*void Slide::setSceneObject(Object * object)
{
    if (! object || object == sceneObject())
        return;

    if (sceneObject())
        disconnect(sceneObject(), SIGNAL(positionChanged(int,int)), this, SLOT(objectPositionChanged(int, int)));
    Action::setSceneObject(object);
    mObjectOriginalPoint = object->sceneRect().topLeft();
    connect(sceneObject(), SIGNAL(positionChanged(int,int)), this, SLOT(objectPositionChanged(int, int)));
}*/


void Slide::finishedEditing()
{
    if (sceneObject() && ! mObjectOriginalPoint.isNull()) {
        sceneObject()->setX(mObjectOriginalPoint.x());
        sceneObject()->setY(mObjectOriginalPoint.y());
    }
}

void Slide::objectPositionChanged(int x, int y)
{
    mObjectOriginalPoint.setX(x);
    mObjectOriginalPoint.setY(y);
}

float Slide::duration() const{
    return mDuration;
}

void Slide::setDuration(float dur) {
    if (mDuration != dur) {
        mDuration = dur;
        emit dataChanged();
    }
}


QVariantMap Slide::toJsonObject()
{
    QVariantMap action = Action::toJsonObject();
    action.insert("startX", mStartPoint.x());
    action.insert("startY", mStartPoint.y());
    action.insert("endX", mEndPoint.x());
    action.insert("endY", mEndPoint.y());
    action.insert("duration", mDuration);

    return action;
}
