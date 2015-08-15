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

#ifndef WAIT_EDITOR_WIDGET_H
#define WAIT_EDITOR_WIDGET_H

#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QDoubleSpinBox>

#include "action_editor_widget.h"
#include "wait.h"

class Wait;

class WaitEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

    QComboBox *mWaitTypeWidget;
    QCheckBox* mSkipBox;
    //QLineEdit* mTimeEdit;
    QDoubleSpinBox* mTimeSpin;

public:
    explicit WaitEditorWidget(QWidget *parent = 0);

protected:
    virtual void updateData(GameObject*);

signals:

public slots:

private slots:
    void onCurrentIndexChanged(int);
    void onTimeChanged(double);
    void onSkipBoxClicked(bool);

private:
    void updateWidgets(int);

};

#endif // WAIT_EDITOR_WIDGET_H
