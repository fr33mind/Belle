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

#include "change_visibility_editor_widget.h"

#include <QHash>
#include <QHashIterator>
#include <QDebug>

#include "scene.h"
#include "fade.h"
#include "slide.h"

ChangeVisibilityEditorWidget::ChangeVisibilityEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    init();
}

void ChangeVisibilityEditorWidget::init()
{
    mObjectsWidget = new QComboBox(this);
    connect(mObjectsWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(onObjectChanged(int)));

    beginGroup(tr("Change Object's Visibility"));
    appendRow(tr("Object"), mObjectsWidget);
    endGroup();

    mFadeEditorWidget = new FadeEditorWidget();
    mSlideEditorWidget = new SlideEditorWidget;
    append(mFadeEditorWidget, 0, QStringList() << "Action" << "Object");
    append(mSlideEditorWidget, 0, QStringList() << "Action" << "Object");

    resizeColumnToContents(0);
}

ChangeVisibilityEditorWidget::~ChangeVisibilityEditorWidget()
{
    mFadeEditorWidget->deleteLater();
    mSlideEditorWidget->deleteLater();
}

void ChangeVisibilityEditorWidget::updateData(Action * action)
{
    ChangeVisibility* changeVisibility = qobject_cast<ChangeVisibility*>(action);
    if (! changeVisibility)
        return;

    ActionEditorWidget::updateData(action);

    mObjects = findObjects(! changeVisibility->toShow());
    mAction = 0;
    mFadeEditorWidget->updateData(changeVisibility->fadeAction());
    mSlideEditorWidget->updateData(changeVisibility->slideAction());

    setGroupName(changeVisibility->name());

    mObjectsWidget->clear();
    for (int i=0; i < mObjects.size(); i++)
        mObjectsWidget->addItem(mObjects[i]->objectName());

    mObjectsWidget->setCurrentIndex(0);
    if (! changeVisibility->sceneObject() && mObjects.size()) {
        changeVisibility->setSceneObject(mObjects[0]);
        //if (lastChangeVisibilityActionForObject(mObjects[0]) == mCurrentAction)
        //    mObjects[0]->setAvailable(! mObjects[0]->isAvailable());
    }

    mAction = action;
}

void ChangeVisibilityEditorWidget::onObjectChanged(int index)
{
    if (! mAction || index >= mObjects.size() || index < 0)
        return;

    /*if (mCurrentAction->sceneObject() && lastChangeVisibilityActionForObject(mCurrentAction->character()) == mCurrentAction) {
        mCurrentAction->character()->setAvailable(! mCurrentAction->character()->isAvailable());
    }*/

    mAction->setSceneObject(mObjects[index]);
    /*if (lastChangeVisibilityActionForObject(mCurrentAction->sceneObject()) == mCurrentAction) {
        mCurrentAction->character()->setAvailable(! mCurrentAction->character()->isAvailable());
    }*/

    emit objectChanged(mAction->sceneObject());
}


Action* ChangeVisibilityEditorWidget::lastChangeVisibilityActionForObject(Object * obj)
{
    if (! mAction)
        return 0;

    Scene *scene = qobject_cast<Scene*>(mAction->parent());
    if (! scene)
        return 0;

    QList<Action*> actions = scene->actions();
    Action * lastAction = 0;
    for(int i=0; i < actions.size(); i++) {
        if (actions[i]->sceneObject() == obj && qobject_cast<ChangeVisibility*>(actions[i])) {
            lastAction = actions[i];
        }
    }

    return lastAction;
}

ChangeVisibility* ChangeVisibilityEditorWidget::currentAction()
{
    return qobject_cast<ChangeVisibility*>(mAction);
}

QList<Object*> ChangeVisibilityEditorWidget::findObjects(bool shown)
{
    Scene* scene = 0;
    QList<Object*> objects;

    if (! mAction)
        return objects;

    scene = mAction->scene();
    if (! scene)
        return objects;

    objects = scene->objects();

    if (mAction->sceneObject()) {
        int index = objects.indexOf(mAction->sceneObject());
        objects.swap(index, 0);
    }

   return objects;
}

