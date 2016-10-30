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

StopSound::StopSound(QObject *parent) :
    Action(parent)
{
    init();
}


StopSound::StopSound(const QVariantMap& data,QObject *parent) :
    Action(data, parent)
{
    init();
    loadInternal(data);
}

void StopSound::init()
{
    setType(GameObjectMetaType::StopSound);
    mSound = 0;
    mFadeTime = 0;
}

void StopSound::loadData(const QVariantMap & data, bool internal)
{
    if (!internal)
        Action::loadData(data, internal);

    if (data.contains("sound") && data.value("sound").type() == QVariant::String)
        setSound(data.value("sound").toString());
    if (data.contains("fadeTime") && data.value("fadeTime").type() == QVariant::Double)
        setFadeTime(data.value("fadeTime").toDouble());
}

void StopSound::setSound(const QString & soundName)
{
    GameObject* obj = ResourceManager::instance()->object(soundName);
    Sound* sound = qobject_cast<Sound*>(obj);
    setSound(sound);
}

void StopSound::setSound(Sound* sound)
{
    if (mSound == sound)
        return;

    if (mSound)
        mSound->disconnect(this);

    mSound = sound;
    QString name;

    if (mSound) {
        connect(mSound, SIGNAL(destroyed()), this, SLOT(onSoundDestroyed()));
        name = mSound->name();
    }

    setDisplayText(name);
}

Sound* StopSound::sound() const
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

QVariantMap StopSound::toJsonObject(bool internal) const
{
    QVariantMap data = Action::toJsonObject(internal);

    data.insert("sound", mSound ? mSound->name() : "");

    if (mFadeTime > 0.0)
        data.insert("fadeTime", mFadeTime);

    return data;
}

void StopSound::onSoundDestroyed()
{
    setSound(0);
}
