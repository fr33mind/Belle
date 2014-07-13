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

#ifndef CHANGE_BACKGROUND_EDITORWIDGET_H
#define CHANGE_BACKGROUND_EDITORWIDGET_H

#include "action_editor_widget.h"
#include "choosefilebutton.h"
#include "change_background.h"
#include "color_pushbutton.h"

class ChangeBackground;

class ChangeBackgroundEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

    ChooseFileButton* mImageChooser;
    ColorPushButton* mChooseBackgroundColorButton;

public:
    explicit ChangeBackgroundEditorWidget(QWidget *parent = 0);
    void updateData(Action *);
    
signals:
    
private slots:
    void onFileSelected(const QString&);
    void onBackgroundColorSelected(const QColor&);
    
};

#endif // CHANGE_BACKGROUND_EDITORWIDGET_H
