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

#include "fade_editor_widget.h"
#include "slide_editor_widget.h"

ChangeVisibilityEditorWidget::ChangeVisibilityEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    init();
}

void ChangeVisibilityEditorWidget::init()
{
    mObjectsWidget = new ObjectComboBox(this);
    connect(mObjectsWidget, SIGNAL(objectChanged(Object*)), this, SLOT(onObjectChanged(Object*)));

    beginGroup(tr("Change Object's Visibility"));
    appendRow(tr("Object"), mObjectsWidget);
    endGroup();

    mFadeCheckBox = new QCheckBox(this);
    connect(mFadeCheckBox, SIGNAL(toggled(bool)), this, SLOT(onFadeToggled(bool)));
    mSlideCheckBox = new QCheckBox(this);
    connect(mSlideCheckBox, SIGNAL(toggled(bool)), this, SLOT(onSlideToggled(bool)));

    beginGroup(tr("Transitions"));
    appendRow(tr("Fade"), mFadeCheckBox);
    appendRow(tr("Slide"), mSlideCheckBox);
    endGroup();

    mFadeEditorWidget = new FadeEditorWidget();
    mSlideEditorWidget = new SlideEditorWidget;
    append(mFadeEditorWidget, 0, QStringList() << "GameObject" << "Object");
    mFadeItemGroup = groupAt(3);
    onFadeToggled(false);

    append(mSlideEditorWidget, 0, QStringList() << "GameObject" << "Object");
    mSlideItemGroup = groupAt(4);
    onSlideToggled(false);

    resizeColumnToContents(0);
}

ChangeVisibilityEditorWidget::~ChangeVisibilityEditorWidget()
{
    mFadeEditorWidget->deleteLater();
    mSlideEditorWidget->deleteLater();
}

void ChangeVisibilityEditorWidget::updateData(GameObject* action)
{
    ActionEditorWidget::updateData(action);
    ChangeVisibility* changeVisibility = qobject_cast<ChangeVisibility*>(action);
    if (! changeVisibility)
        return;

    mFadeEditorWidget->setGameObject(changeVisibility->fadeAction());
    mSlideEditorWidget->setGameObject(changeVisibility->slideAction());

    if (changeVisibility->isFadeActionEnabled())
        mFadeCheckBox->setChecked(true);
    if (changeVisibility->isSlideActionEnabled())
        mSlideCheckBox->setChecked(true);

    const GameObjectMetaType* metatype = GameObjectMetaType::metaType(changeVisibility->type());
    QString typeName = metatype ? metatype->name() : "";
    QStandardItem* item = groupAt(1);
    if (item)
        item->setText(typeName);

    mObjectsWidget->loadFromAction(changeVisibility);
}

void ChangeVisibilityEditorWidget::onObjectChanged(Object* obj)
{
    ChangeVisibility* changeVisibility = qobject_cast<ChangeVisibility*>(mGameObject);
    if (! changeVisibility)
        return;

    /*if (mCurrentAction->sceneObject() && lastChangeVisibilityActionForObject(mCurrentAction->character()) == mCurrentAction) {
        mCurrentAction->character()->setAvailable(! mCurrentAction->character()->isAvailable());
    }*/

    changeVisibility->setSceneObject(obj);
    /*if (lastChangeVisibilityActionForObject(mCurrentAction->sceneObject()) == mCurrentAction) {
        mCurrentAction->character()->setAvailable(! mCurrentAction->character()->isAvailable());
    }*/

    emit objectChanged(changeVisibility->sceneObject());
}


Action* ChangeVisibilityEditorWidget::lastChangeVisibilityActionForObject(Object * obj)
{
    if (! mGameObject)
        return 0;

    Scene *scene = qobject_cast<Scene*>(mGameObject->parent());
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
    return qobject_cast<ChangeVisibility*>(mGameObject);
}

void ChangeVisibilityEditorWidget::onFadeToggled(bool checked)
{
    ChangeVisibility* currAction = currentAction();
    if (currAction) {
        currAction->setFadeActionEnabled(checked);
        if (checked)
            mFadeEditorWidget->setGameObject(currAction->fadeAction());
    }

    if (mFadeItemGroup)
        setRowHidden(mFadeItemGroup->row(), mFadeItemGroup->index().parent(), !checked);
}

void ChangeVisibilityEditorWidget::onSlideToggled(bool checked)
{
    ChangeVisibility* currAction = currentAction();
    if (currAction) {
        currAction->setSlideActionEnabled(checked);
        if (checked)
            mSlideEditorWidget->setGameObject(currAction->slideAction());
    }

    if (mSlideItemGroup)
        setRowHidden(mSlideItemGroup->row(), mSlideItemGroup->index().parent(), !checked);
}
