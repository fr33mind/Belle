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

#include "wait_editor_widget.h"

#include <QDebug>

WaitEditorWidget::WaitEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mWaitTypeWidget = new QComboBox(this);
    mWaitTypeWidget->addItem(tr("Timed"));
    mWaitTypeWidget->addItem(tr("Until mouse button is clicked"));
    mWaitTypeWidget->addItem(tr("Forever"));

    mSkipBox = new QCheckBox(this);
    mTimeSpin = new QDoubleSpinBox(this);
    mTimeSpin->setValue(1);
    mTimeSpin->setSingleStep(0.1);
    mTimeSpin->setMaximum(3600);

    beginGroup(tr("Wait"));
    appendRow(tr("Type"), mWaitTypeWidget);
    appendRow(tr("Time (sec)"), mTimeSpin);
    appendRow(tr("Allow skipping"), mSkipBox);
    endGroup();

    resizeColumnToContents(0);

    connect(mWaitTypeWidget, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    connect(mTimeSpin, SIGNAL(valueChanged(double)), this, SLOT(onTimeChanged(double)));
    connect(mSkipBox, SIGNAL(clicked(bool)), this, SLOT(onSkipBoxClicked(bool)));
}

void WaitEditorWidget::updateData(Action * action)
{
    Wait* wait = qobject_cast<Wait*>(action);
    if (! wait)
        return;
    ActionEditorWidget::updateData(action);
    mAction = 0;

    mWaitTypeWidget->setCurrentIndex(wait->waitType());
    updateWidgets(mWaitTypeWidget->currentIndex());
    mTimeSpin->setValue(wait->time());
    mSkipBox->setChecked(wait->allowSkipping());
    mAction = action;
}

void WaitEditorWidget::onCurrentIndexChanged(int index)
{
    Wait* wait = qobject_cast<Wait*>(mAction);
    if (! wait)
        return;

    wait->setWaitTypeFromIndex(index);
    updateWidgets(index);
}

void WaitEditorWidget::onTimeChanged(double value)
{
    Wait* wait = qobject_cast<Wait*>(mAction);
    if (wait)
        wait->setTime(value);
}

void WaitEditorWidget::updateWidgets(int index)
{
    bool enabled = index >= 1 ? false : true;

    mSkipBox->setEnabled(enabled);
    mTimeSpin->setEnabled(enabled);
}

void WaitEditorWidget::onSkipBoxClicked(bool clicked)
{
    if (mAction)
        mAction->setAllowSkipping(clicked);
}
