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

#ifndef CHANGE_VISIBILITY_EDITOR_WIDGET_H
#define CHANGE_VISIBILITY_EDITOR_WIDGET_H

#include <QComboBox>
#include <QCheckBox>

#include "action_editor_widget.h"
#include "resource_manager.h"
#include "change_visibility.h"
#include "object_combobox.h"

class ChangeVisibility;
class Fade;
class Slide;
class FadeEditorWidget;
class SlideEditorWidget;

class ChangeVisibilityEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

protected:
    ObjectComboBox * mObjectsWidget;
    QList<Object*> mObjects;
    FadeEditorWidget* mFadeEditorWidget;
    SlideEditorWidget* mSlideEditorWidget;
    QCheckBox* mFadeCheckBox;
    QCheckBox* mSlideCheckBox;
    QStandardItem* mFadeItemGroup;
    QStandardItem* mSlideItemGroup;

public:
    explicit ChangeVisibilityEditorWidget(QWidget *parent = 0);
    ~ChangeVisibilityEditorWidget();

    ChangeVisibility* currentAction();

    QComboBox* charactersWidget();

    QList<Character*> characters();
    void setCharacters(const QList<Character*>&);

private slots:
    void onObjectChanged(Object*);
    void onFadeToggled(bool);
    void onSlideToggled(bool);

protected:
    virtual void updateData(GameObject*);

private:
    void init();
    void setFadeTypeWidgetDisabled(bool);
    Action* lastChangeVisibilityActionForObject(Object*);
    
signals:
    void objectChanged(Object*);
    
public slots:
    
};

#endif // ChangeVisibility_EDITOR_WIDGET_H
