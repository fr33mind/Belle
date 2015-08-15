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

#include "show_editor_widget.h"

#include "fade_editor_widget.h"
#include "slide_editor_widget.h"
#include "scene.h"

ShowEditorWidget::ShowEditorWidget(QWidget *parent):
    ChangeVisibilityEditorWidget(parent)
{
    mObjectStateWidget = new QComboBox(this);
    appendRow(tr("Initial State"), mObjectStateWidget);
    connect(mObjectStateWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(onStateChanged(int)));
    connect(this, SIGNAL(objectChanged(Object*)), this , SLOT(onObjectChanged(Object*)));
}

void ShowEditorWidget::updateData(GameObject* action)
{
    ChangeVisibilityEditorWidget::updateData(action);
    Show* show = qobject_cast<Show*>(action);
    if (! show)
        return;

    mObjectStateWidget->clear();
    Character* character = qobject_cast<Character*>(show->sceneObject());
    if (character) {
        mObjectStateWidget->addItems(character->states());
        int currIndex = mObjectStateWidget->findText(show->characterState());
        if (currIndex != -1)
            mObjectStateWidget->setCurrentIndex(currIndex);
        else
            mObjectStateWidget->setCurrentIndex(-1);
    }
}

void ShowEditorWidget::setGameObject(GameObject* action)
{
    ChangeVisibilityEditorWidget::setGameObject(action);

    Show* show = qobject_cast<Show*>(action);
    if (show && show->sceneObject() && show->characterState().isEmpty())
        mObjectStateWidget->setCurrentIndex(0);
}

void ShowEditorWidget::onStateChanged(int index)
{
    Show* show = qobject_cast<Show*>(mGameObject);
    if (show && show->sceneObject()) {
        Character *character = qobject_cast<Character*>(show->sceneObject());
        if (character)
            show->setCharacterState(mObjectStateWidget->currentText());
        else
            show->setCharacterState("");
    }
}

void ShowEditorWidget::onObjectChanged(Object * obj)
{
    Character *character = qobject_cast<Character*>(obj);

    mObjectStateWidget->clear();
    if (character) {
        mObjectStateWidget->setEnabled(true);
        mObjectStateWidget->addItems(character->statesToPaths().keys());
    }
    else {
        mObjectStateWidget->setEnabled(false);
    }
}
