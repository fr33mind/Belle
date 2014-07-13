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
#include <QDebug>

static PlaySoundEditorWidget * mEditorWidget = 0;
ActionInfo PlaySound::Info;

PlaySound::PlaySound(QObject *parent) :
    Action(parent)
{
    init();
}

PlaySound::PlaySound(const QVariantMap& data,QObject *parent) :
    Action(data, parent)
{
    init();

    if (data.contains("sound") && data.value("sound").type() == QVariant::String)
        setSoundPath(data.value("sound").toString());

    if (data.contains("volume") && data.value("volume").canConvert(QVariant::Int))
        mVolume = data.value("volume").toInt();

    if (data.contains("loop") && data.value("loop").type() == QVariant::Bool)
        mLoop = data.value("loop").toBool();
}

void PlaySound::init()
{
    setType(Info.type);
    setName(Info.name);
    setIcon(Info.icon);
    mVolume = 100;
    mSoundName = "";
    mLoop = false;
    connect(this, SIGNAL(objectNameChanged()), this, SLOT(onObjectNameChanged()));
}

void PlaySound::setPlaySoundEditorWidget(PlaySoundEditorWidget * widget)
{
   mEditorWidget = widget;
}

PlaySoundEditorWidget* PlaySound::playSoundEditorWidget()
{
   return mEditorWidget;
}

ActionEditorWidget* PlaySound::editorWidget()
{
    return mEditorWidget;
}

void PlaySound::setSoundPath(const QString & path)
{
    mSoundName = ResourceManager::newSound(path);
    if (! path.isEmpty()) {
        QString name = "";
        if (! objectName().isEmpty())
            name += objectName() + ": ";

        setDisplayText(name + mSoundName);
    }
}

QString PlaySound::soundPath()
{
    return ResourceManager::mediaPath(mSoundName);
}

QString PlaySound::soundName()
{
    return mSoundName;
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

void PlaySound::onObjectNameChanged()
{
    QString name = "";
    if (! objectName().isEmpty())
        name += objectName() + ": ";

    setDisplayText(name + mSoundName);
}

QVariantMap PlaySound::toJsonObject()
{
    QVariantMap action = Action::toJsonObject();
    action.insert("sound", mSoundName);
    action.insert("volume", mVolume);
    action.insert("loop", mLoop);
    return action;
}

