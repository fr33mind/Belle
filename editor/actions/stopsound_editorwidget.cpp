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

#include "stopsound_editorwidget.h"

#include <QDebug>

#include "resource_manager.h"

StopSoundEditorWidget::StopSoundEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mSoundComboBox = new GameObjectComboBox(this);
    mSoundComboBox->setIconsEnabled(true);

    mFadeOutSpinBox = new QDoubleSpinBox(this);
    mFadeOutSpinBox->setMaximum(10);
    mFadeOutSpinBox->setSingleStep(0.1);

    beginGroup(tr("Stop Sound Editor"));
    appendRow(tr("Sound"), mSoundComboBox);
    appendRow(QString("%1 %2").arg(tr("Fade out duration")).arg("(s)") , mFadeOutSpinBox);
    endGroup();

    connect(mSoundComboBox, SIGNAL(objectChanged(GameObject*)), this, SLOT(onSoundChanged(GameObject*)));
    connect(mFadeOutSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onFadeChanged(double)));

    resizeColumnToContents(0);
}

void StopSoundEditorWidget::updateData(GameObject* action)
{
    ActionEditorWidget::updateData(action);
    StopSound * stopSound = qobject_cast<StopSound*>(action);
    if (! stopSound)
        return;

    QList<GameObject*> sounds = ResourceManager::instance()->objects(GameObjectMetaType::Sound);
    mSoundComboBox->setObjects(sounds, stopSound->sound());
    mFadeOutSpinBox->setValue(stopSound->fadeTime());
}

void StopSoundEditorWidget::onSoundChanged(GameObject* sound)
{
    StopSound * stopSound = qobject_cast<StopSound*>(mGameObject);
    if (! stopSound)
        return;

    stopSound->setSound(qobject_cast<Sound*>(sound));
}


void StopSoundEditorWidget::onFadeChanged(double val)
{
    StopSound * stopSound = qobject_cast<StopSound*>(mGameObject);
    if (stopSound)
        stopSound->setFadeTime(val);
}
