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

Slide::Slide(QObject *parent) :
    Action(parent)
{
    init();
}

Slide::Slide(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();
    loadInternal(data);
}


void Slide::init()
{
    setType(GameObjectMetaType::Slide);
    setSupportedEvents(Interaction::MousePress | Interaction::MouseRelease |
    Interaction::MouseMove);
    mObjectOriginalPoint = QPoint();
    mSlideType = Slide::In;
    mDestX = "";
    mDestY = "";
    mDuration = 1;
}

void Slide::loadData(const QVariantMap & data, bool internal)
{
    if (!internal)
        Action::loadData(data, internal);

    if (data.contains("destX") && data.value("destX").canConvert(QVariant::String))
        setDestX(data.value("destX").toString());

    if (data.contains("destY") && data.value("destY").canConvert(QVariant::String))
        setDestY(data.value("destY").toString());

    if (data.contains("slideType") && data.value("slideType").canConvert(QVariant::String))
        setSlideType(data.value("slideType").toString());

    if (data.contains("duration") && data.value("duration").canConvert(QVariant::Double))
        setDuration(data.value("duration").toDouble());
}

QString Slide::displayText() const
{
    if (! this->sceneObject() || mDestX.isEmpty() && mDestY.isEmpty())
        return "";

    return QString("Slide %1 \"%2\" to (%3, %4)").arg(slideTypeAsString())
                                                .arg(this->sceneObject()->objectName())
                                                .arg(mDestX)
                                                .arg(mDestY);
}

Slide::Type Slide::slideType() const
{
    return mSlideType;
}

QString Slide::slideTypeAsString() const
{
    switch(mSlideType) {
        case Slide::In: return "in";
        case Slide::Out: return "out";
        case Slide::Custom: return "custom";
    }

    return "";
}

void Slide::setSlideType(Slide::Type type)
{
    if (mSlideType == type)
        return;

    mSlideType = type;
    emit dataChanged();
}

void Slide::setSlideType(const QString& type)
{
    QString t = type.toLower();
    if (t == "in")
        mSlideType = Slide::In;
    else if (t == "out")
        mSlideType = Slide::Out;
    else if(t == "custom")
        mSlideType = Slide::Custom;
    else
        return;
    emit dataChanged();
}

void Slide::updateSlideType()
{
    bool ok1, ok2;
    mDestX.toInt(&ok1);
    mDestY.toInt(&ok2);
    if (ok1 || ok2)
        this->setSlideType(Slide::Custom);
    else
        this->setSlideType(Slide::In);
}

void Slide::setDestX(const QString& pos)
{
    if (mDestX == pos)
        return;

    mDestX = pos;
    emit dataChanged();
}

void Slide::setDestX(int x)
{
    if (slideType() == Slide::Custom)
        setDestX(QString::number(x));
}

QString Slide::destX() const
{
    return mDestX;
}

void Slide::setDestY(const QString& pos)
{
    if (mDestY == pos)
        return;

    mDestY = pos;
    emit dataChanged();
}

void Slide::setDestY(int y)
{
    if (slideType() == Slide::Custom)
        setDestY(QString::number(y));
}

QString Slide::destY() const
{
    return mDestY;
}

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

double Slide::duration() const
{
    return mDuration;
}

void Slide::setDuration(double dur) {
    if (mDuration != dur) {
        mDuration = dur;
        emit dataChanged();
    }
}

QVariantMap Slide::toJsonObject(bool internal) const
{
    QVariantMap action = Action::toJsonObject(internal);
    action.insert("slideType", slideTypeAsString());
    action.insert("destX", mDestX);
    action.insert("destY", mDestY);
    action.insert("duration", mDuration);

    return action;
}
