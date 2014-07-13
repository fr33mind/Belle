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

#include "fade_editor_widget.h"

#include <QComboBox>
#include <QDebug>

#include "scene_manager.h"

FadeEditorWidget::FadeEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    //ResourcesComboBox * resourcesChooser = new ResourcesComboBox(this);
    mObjectChooser = new QComboBox(this);
    mFadeTypeChooser = new QComboBox(this);
    mFadeTypeChooser->addItems(QStringList() << tr("In") << tr("Out"));
    mDurationSpin = new QDoubleSpinBox(this);
    mDurationSpin->setRange(0, 30);
    mDurationSpin->setSingleStep(0.1);

    connect(mFadeTypeChooser, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentFadeTypeChanged(int)));
    connect(mObjectChooser, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentObjectChanged(int)));
    connect(mDurationSpin, SIGNAL(valueChanged(double)), SLOT(onDurationChanged(double)));

    beginGroup(tr("Fade Action"));
    appendRow(tr("Object"), mObjectChooser, "Object");
    appendRow(tr("Fade"), mFadeTypeChooser);
    appendRow(tr("Duration"), mDurationSpin);
    endGroup();
}


void FadeEditorWidget::updateData(Action * action)
{
    Fade* fade = qobject_cast<Fade*>(action);
    if (! fade)
        return;
    ActionEditorWidget::updateData(action);
    mAction = 0;

    QList<Object*> objects;
    mObjectChooser->clear();
    if (fade->sceneObject()) {
        mObjectChooser->addItem(fade->sceneObject()->objectName());
        objects.append(fade->sceneObject());
    }

    Scene* scene = fade->scene();
    if (scene) {
        QList<Object*> objs = scene->objects();
        for (int i=0; i < objs.size(); i++) {
            if (objs[i] && objs[i] != fade->sceneObject()) {
                mObjectChooser->addItem(objs[i]->objectName());
                objects.append(objs[i]);
            }
        }
    }

    setObjects(objects);

    if (! fade->sceneObject() && ! objects.isEmpty())
        fade->setSceneObject(objects[0]);

    mFadeTypeChooser->setCurrentIndex(fade->fadeType());
    mDurationSpin->setValue(fade->duration());
    mAction = action;
}

void FadeEditorWidget::onCurrentFadeTypeChanged(int type)
{
    Fade* fade = qobject_cast<Fade*>(mAction);
    if (fade)
        type ? fade->setFadeType(Fade::Out) : fade->setFadeType(Fade::In);
}

void FadeEditorWidget::onCurrentObjectChanged(int index)
{
    if (!mAction || index < 0 || index >= objects().size())
        return;

    mAction->setSceneObject(objects()[index]);
}

void FadeEditorWidget::onDurationChanged(double time)
{
    Fade* fade = qobject_cast<Fade*>(mAction);
    if (fade)
        fade->setDuration(time);
}
