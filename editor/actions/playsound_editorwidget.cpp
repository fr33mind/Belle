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

#include "playsound_editorwidget.h"

#include <QtDebug>

PlaySoundEditorWidget::PlaySoundEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mSoundComboBox = new GameObjectComboBox(this);
    mSoundComboBox->setIconsEnabled(true);
    mVolumeSlider = new QSlider(Qt::Horizontal);
    mVolumeSlider->setMinimum(0);
    mVolumeSlider->setMaximum(100);
    mLoopCheckBox = new QCheckBox(this);

    connect(mSoundComboBox, SIGNAL(objectChanged(GameObject*)), this, SLOT(onSoundChanged(GameObject*)));
    connect(mVolumeSlider, SIGNAL(valueChanged(int)), this, SLOT(onVolumeChanged(int)));
    connect(mLoopCheckBox, SIGNAL(toggled(bool)), this, SLOT(onLoopToggled(bool)));

    beginGroup(tr("Play Sound Action"));
    appendRow(tr("Sound"), mSoundComboBox);
    appendRow(tr("Volume"), mVolumeSlider);
    appendRow(tr("Loop"), mLoopCheckBox);
    endGroup();
}

void PlaySoundEditorWidget::updateData(GameObject* action)
{
    ActionEditorWidget::updateData(action);
    PlaySound* playSound = qobject_cast<PlaySound*>(action);
    if (! playSound)
        return;

    QList<GameObject*> sounds = ResourceManager::instance()->objects(GameObjectMetaType::Sound);
    mSoundComboBox->setObjects(sounds, playSound->sound());
    mVolumeSlider->setValue(playSound->volume());
    mLoopCheckBox->setChecked(playSound->loop());
}

void PlaySoundEditorWidget::onSoundChanged(GameObject* sound)
{
    PlaySound* playSound = qobject_cast<PlaySound*>(mGameObject);
    if (playSound) {
        playSound->setSound(qobject_cast<Sound*>(sound));
    }
}

void PlaySoundEditorWidget::onVolumeChanged(int vol)
{
    PlaySound* playSound = qobject_cast<PlaySound*>(mGameObject);
    if (playSound)
        playSound->setVolume(vol);
}

void PlaySoundEditorWidget::onLoopToggled(bool checked)
{
    PlaySound* playSound = qobject_cast<PlaySound*>(mGameObject);
    if (playSound)
        playSound->setLoop(checked);
}
