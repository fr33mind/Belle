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

#ifndef CHANGEOBJECTBACKGROUND_EDITORWIDGET_H
#define CHANGEOBJECTBACKGROUND_EDITORWIDGET_H

#include <QCheckBox>

#include "action_editor_widget.h"
#include "widgets/object_combobox.h"
#include "color_pushbutton.h"
#include "choosefilebutton.h"

class ChangeObjectBackground;

class ChangeObjectBackgroundEditorWidget : public ActionEditorWidget
{
    Q_OBJECT
public:
    explicit ChangeObjectBackgroundEditorWidget(QWidget *parent = 0);

signals:

protected:
    virtual void updateData(GameObject*);

private slots:
    void onCurrentObjectChanged(Object*);
    void onColorChosen(const QColor&);
    void onOpacityChanged(int);
    void onFileSelected(const QString&);
    void onImageCheckBoxToggled(bool);
    void onColorCheckBoxToggled(bool);
    void onOpacityCheckBoxToggled(bool);

private:
    ObjectComboBox* mObjectsComboBox;
    ColorPushButton* mColorButton;
    QSlider * mOpacitySlider;
    QList<Object*> mObjects;
    ChooseFileButton* mImageChooser;
    QCheckBox* mImageCheckBox;
    QCheckBox* mColorCheckBox;
    QCheckBox* mOpacityCheckBox;
};

#endif // CHANGEOBJECTBACKGROUND_EDITORWIDGET_H
