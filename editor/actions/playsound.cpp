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

#include "playsound.h"

#include <QFileInfo>

PlaySound::PlaySound(QObject *parent) :
    Action(parent)
{
    init();
}

PlaySound::PlaySound(const QVariantMap& data,QObject *parent) :
    Action(data, parent)
{
    init();

    if (data.contains("sound") && data.value("sound").type() == QVariant::String) {
        setSound(data.value("sound").toString());
    }

    if (data.contains("volume") && data.value("volume").canConvert(QVariant::Int))
        mVolume = data.value("volume").toInt();

    if (data.contains("loop") && data.value("loop").type() == QVariant::Bool)
        mLoop = data.value("loop").toBool();
}

void PlaySound::init()
{
    setType(GameObjectMetaType::PlaySound);
    mVolume = 100;
    mSound = 0;
    mLoop = false;
}

void PlaySound::setSound(const QString& soundName)
{
    GameObject* obj = ResourceManager::instance()->object(soundName);
    Sound* sound = qobject_cast<Sound*>(obj);
    setSound(sound);
}

void PlaySound::setSound(Sound* sound)
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

Sound* PlaySound::sound() const
{
    return mSound;
}

void PlaySound::setVolume(int vol)
{
    mVolume = vol;
}

int PlaySound::volume()
{
    return mVolume;
}

void PlaySound::setLoop(bool loop)
{
    mLoop = loop;
}

bool PlaySound::loop()
{
    return mLoop;
}

QVariantMap PlaySound::toJsonObject(bool internal) const
{
    QVariantMap action = Action::toJsonObject(internal);
    action.insert("sound", mSound ? mSound->name() : "");
    action.insert("volume", mVolume);
    action.insert("loop", mLoop);
    return action;
}

void PlaySound::onSoundDestroyed()
{
    setSound(0);
}
