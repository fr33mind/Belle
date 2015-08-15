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

#ifndef LABEL_EDITOR_WIDGET_H
#define LABEL_EDITOR_WIDGET_H

#include "label.h"

class Label;

class LabelEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

    QLineEdit *mLabelEdit;

public:
    explicit LabelEditorWidget(QWidget *parent = 0);
    
signals:

protected:
    virtual void updateData(GameObject*);
    
private slots:
    void onLabelEdited(const QString&);

};

#endif // LABEL_EDITOR_WIDGET_H
