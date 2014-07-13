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

#ifndef STOPSOUND_EDITORWIDGET_H
#define STOPSOUND_EDITORWIDGET_H

#include "action_editor_widget.h"
#include "stopsound.h"

class StopSound;

class StopSoundEditorWidget : public ActionEditorWidget
{
    Q_OBJECT

    QComboBox * mSoundEdit;
    QDoubleSpinBox* mFadeOutSpinBox;

public:
    explicit StopSoundEditorWidget(QWidget *parent = 0);
    virtual void updateData(Action *);

signals:

public slots:

private slots:
    void onSoundChanged(const QString&);
    void onFadeChanged(double);

};

#endif // STOPSOUND_EDITORWIDGET_H
