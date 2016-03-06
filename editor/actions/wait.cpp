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

#include "wait.h"

ActionInfo Wait::Info;

Wait::Wait(QObject *parent) :
    Action(parent)
{
    init();
}

Wait::Wait(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();

    if (data.contains("time") && data.value("time").canConvert(QVariant::Double))
        setTime(data.value("time").toDouble());

    if (data.contains("waitType") && data.value("waitType").type() == QVariant::String) {
        QString type = data.value("waitType").toString();

        if (type ==  "Timed")
            setWaitType(Wait::Timed);
        else if (type == "Forever")
            setWaitType(Wait::Forever);
        else
            setWaitType(Wait::MouseClick);
    }

    //setValue(waitTypeToString(mWaitType));
}

void Wait::init()
{
    setType(Info.type);
    setTypeName(Info.typeName);
    setIcon(Info.icon);
    setWaitType(Timed);
    setTime(1);
}

void Wait::setTime(double t)
{
    mTime = t;
    if (mWaitType == Timed)
        setDisplayText(QString::number(mTime) + tr(" second(s)"));
}

double Wait::time()
{
    return mTime;
}

void Wait::setWaitType(WaitType type)
{
    mWaitType = type;

    if (type == Timed)
        setTime(mTime);
    else if (type == MouseClick)
        setDisplayText(tr("Until mouse button is clicked"));
    else
        setDisplayText(tr("Forever"));
}

QString Wait::waitTypeToString(WaitType type) const
{
    switch(type) {
    case Timed:
        return "Timed";
    case MouseClick:
        return "MouseClick";
    case Forever:
        return "Forever";
    }

    return "";
}

void Wait::setWaitTypeFromIndex(int index)
{
    switch (index) {
    case 0:
        setWaitType(Timed);
        break;
    case 1:
        setWaitType(MouseClick);
        break;
    case 2:
        setWaitType(Forever);
        break;
    }
}

Wait::WaitType Wait::waitType()
{
    return mWaitType;
}

QVariantMap Wait::toJsonObject(bool internal) const
{
    QVariantMap action = Action::toJsonObject(internal);
    if (mWaitType == Wait::Timed)
        action.insert("time", mTime);
    action.insert("waitType", waitTypeToString(mWaitType));
    return action;
}
