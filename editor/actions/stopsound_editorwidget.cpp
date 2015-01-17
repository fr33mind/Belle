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
    mSoundEdit = new QComboBox(this);
    mSoundEdit->setEditable(true);

    mFadeOutSpinBox = new QDoubleSpinBox(this);
    mFadeOutSpinBox->setMaximum(10);
    mFadeOutSpinBox->setSingleStep(0.1);

    beginGroup(tr("Stop Sound Editor"));
    appendRow(tr("Sound"), mSoundEdit);
    appendRow(QString("%1 %2").arg(tr("Fade out duration")).arg("(s)") , mFadeOutSpinBox);
    endGroup();

    connect(mSoundEdit, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onSoundChanged(const QString&)));
    connect(mFadeOutSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onFadeChanged(double)));

    resizeColumnToContents(0);
}

void StopSoundEditorWidget::updateData(Action * action)
{
    StopSound * stopSound = qobject_cast<StopSound*>(action);
    if (! stopSound)
        return;

    ActionEditorWidget::updateData(stopSound);
    mAction = 0;

    QString currSound = stopSound->sound();
    QList<Asset*> sounds = AssetManager::instance()->assets(Asset::Audio);

    mSoundEdit->clear();
    if (! currSound.isEmpty())
        mSoundEdit->addItem(currSound);

    for(int i=0; i < sounds.size(); i++) {
        if (sounds[i]->name() != stopSound->sound())
            mSoundEdit->insertItem(i, sounds[i]->name());
    }
    mSoundEdit->setCurrentIndex(0);
    mFadeOutSpinBox->setValue(stopSound->fadeTime());

    if (stopSound->sound().isEmpty() && ! sounds.isEmpty())
        stopSound->setSound(sounds.first()->name());

    mAction = action;
}


void StopSoundEditorWidget::onSoundChanged(const QString& text)
{
    StopSound * stopSound = qobject_cast<StopSound*>(mAction);
    if (! stopSound)
        return;

    stopSound->setSound(text);
}


void StopSoundEditorWidget::onFadeChanged(double val)
{
    StopSound * stopSound = qobject_cast<StopSound*>(mAction);
    if (stopSound)
        stopSound->setFadeTime(val);
}
