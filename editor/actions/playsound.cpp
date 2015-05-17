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
    setTypeName(Info.typeName);
    setIcon(Info.icon);
    mVolume = 100;
    mSound = 0;
    mLoop = false;
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
    if (mSound && mSound->path() == path)
        return;

    mSound = AssetManager::instance()->loadAsset(path, Asset::Audio);
}

QString PlaySound::soundPath()
{
    if (mSound)
        return mSound->path();
    return "";
}

QString PlaySound::soundName()
{
    if (mSound)
        return mSound->name();
    return "";
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

QVariantMap PlaySound::toJsonObject()
{
    QVariantMap action = Action::toJsonObject();
    action.insert("sound", mSound ? mSound->name() : "");
    action.insert("volume", mVolume);
    action.insert("loop", mLoop);
    return action;
}

QString PlaySound::displayText() const
{
    if (! mSound)
        return "";

    QString displayText = objectName();
    if (! displayText.isEmpty())
        displayText += ": ";
    displayText += mSound->name();

    return displayText;
}
