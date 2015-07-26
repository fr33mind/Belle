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

#include "stopsound.h"

#include "scene_manager.h"

ActionInfo StopSound::Info;

StopSound::StopSound(QObject *parent) :
    Action(parent)
{
    init();
}


StopSound::StopSound(const QVariantMap& data,QObject *parent) :
    Action(data, parent)
{
    init();

    if (data.contains("sound") && data.value("sound").type() == QVariant::String)
        setSound(data.value("sound").toString());
    if (data.contains("fadeTime") && data.value("fadeTime").type() == QVariant::Double)
        setFadeTime(data.value("fadeTime").toDouble());
}

void StopSound::init()
{
    setType(Info.type);
    setTypeName(Info.typeName);
    setIcon(Info.icon);
    mSound = "";
    mFadeTime = 0;
}

void StopSound::setSound(const QString & sound)
{
    if (mSound != sound) {
        mSound = sound;
        setDisplayText(sound);
    }
}

QString StopSound::sound()
{
    return mSound;
}

void StopSound::setFadeTime(double time)
{
    mFadeTime = time;
}

double StopSound::fadeTime()
{
    return mFadeTime;
}

QVariantMap StopSound::toJsonObject()
{
    QVariantMap data = Action::toJsonObject();

    data.insert("sound", mSound);

    if (mFadeTime > 0.0)
        data.insert("fadeTime", mFadeTime);

    return data;
}
