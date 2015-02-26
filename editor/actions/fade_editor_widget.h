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

#ifndef FADE_EDITOR_WIDGET_H
#define FADE_EDITOR_WIDGET_H

#include "fade.h"
#include "resource_manager.h"
#include "resources_combobox.h"
#include "widgets/object_combobox.h"

class Fade;

class FadeEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

    ObjectComboBox* mObjectChooser;
    QComboBox* mFadeTypeChooser;
    QDoubleSpinBox* mDurationSpin;

public:
    explicit FadeEditorWidget(QWidget *parent = 0);
    virtual void updateData(Action*);

signals:

private slots:
    void onCurrentFadeTypeChanged(int);
    void onCurrentObjectChanged(Object*);
    void onDurationChanged(double);

};

#endif // FADE_EDITOR_WIDGET_H
