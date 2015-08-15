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

#ifndef SLIDE_EDITOR_WIDGET_H
#define SLIDE_EDITOR_WIDGET_H

#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>

#include "action_editor_widget.h"
#include "resource_manager.h"
#include "slide.h"
#include "object_combobox.h"

class Slide;

class SlideEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

    QDoubleSpinBox*  mDurationSpinBox;
    ObjectComboBox* mObjectChooser;
    QComboBox* mDestXChooser;
    QAbstractItemModel* mDestXChooserEmptyModel;
    QAbstractItemModel* mDestXChooserModel;
    QComboBox* mDestYChooser;
    QComboBox* mTypeChooser;
    bool mIsDown;

public:
    explicit SlideEditorWidget(QWidget *parent = 0);
    virtual bool eventFilter(QObject *, QEvent *);

protected:
    virtual void updateData(GameObject*);

signals:


private:
    void setupDestChooser(int type=0);

private slots:
    void onButtonClicked();
//    void onSliderValueChanged(int);
    void onObjectChanged(Object*);
    void onTypeChanged(int);
    void onDestXChanged(const QString&);
    void onDestYChanged(const QString&);
    void onDestXChanged(int);
    void onDestYChanged(int);
    void onDurationChanged(double);

};

#endif // SLIDE_EDITOR_WIDGET_H
