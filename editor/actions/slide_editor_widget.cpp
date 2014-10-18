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

#include "slide_editor_widget.h"

#include <QLineEdit>
#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QIntValidator>

#include "scene_manager.h"

SlideEditorWidget::SlideEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mObjectChooser = new QComboBox(this);
    mTypeChooser = new QComboBox(this);
    mTypeChooser->addItems(QStringList() << tr("In") << tr("Out") << tr("Custom"));
    mDurationSpinBox = new QDoubleSpinBox(this);
    mDurationSpinBox->setSingleStep(0.1);
    mDurationSpinBox->setMinimum(0);

    QWidget * positionWidget = new QWidget(this);
    QHBoxLayout* hLayout = new QHBoxLayout(positionWidget);
    mDestXChooser = new QComboBox(positionWidget);
    mDestYChooser = new QComboBox(positionWidget);

    //configure position choosers
    setupDestChooser();
    mDestXChooser->setValidator(new QIntValidator(mDestXChooser));
    mDestYChooser->setValidator(new QIntValidator(mDestYChooser));

    hLayout->addWidget(mDestXChooser);
    hLayout->addWidget(mDestYChooser);
    beginGroup(tr("Slide Action"));
    appendRow(tr("Object"), mObjectChooser, "Object");
    appendRow(tr("Type"), mTypeChooser);
    appendRow(tr("Destination"), positionWidget);
    appendRow(tr("Duration (sec)"),  mDurationSpinBox);
    endGroup();

    connect(mObjectChooser, SIGNAL(currentIndexChanged(int)), this, SLOT(onResourceChanged(int)));
    connect(mTypeChooser, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypeChanged(int)));
    connect(mDestXChooser, SIGNAL(editTextChanged(const QString &)), this, SLOT(onDestXChanged(const QString &)));
    connect(mDestYChooser, SIGNAL(editTextChanged(const QString &)), this, SLOT(onDestYChanged(const QString &)));
    connect(mDestXChooser, SIGNAL(currentIndexChanged(int)), this, SLOT(onDestXChanged(int)));
    connect(mDestYChooser, SIGNAL(currentIndexChanged(int)), this, SLOT(onDestYChanged(int)));
    connect(mDurationSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onDurationChanged(double)));

    mIsDown = false;
    resizeColumnToContents(0);
}

void SlideEditorWidget::updateData(Action * action)
{
    Slide * slide  = qobject_cast<Slide*>(action);
    if (! slide || ! slide->scene())
        return;

    ActionEditorWidget::updateData(action);
    mAction = 0;

    mDurationSpinBox->setValue(slide->duration());

    mObjectChooser->clear();

    Object* currObj = slide->sceneObject();
    QList<Object*> objects;
    if (currObj) {
        mObjectChooser->addItem(currObj->objectName());
        objects.append(currObj);
    }

    QList<Object*> objs = slide->scene()->objects();
    for (int i=0; i < objs.size(); i++) {
        if (objs[i] && objs[i] != currObj) {
            mObjectChooser->addItem(objs[i]->objectName());
            objects.append(objs[i]);
        }
    }

    this->setObjects(objects);

    if (! currObj && ! objects.isEmpty())
        slide->setSceneObject(objects[0]);


    mTypeChooser->setCurrentIndex(static_cast<int>(slide->slideType()));
    bool custom = (slide->slideType() == Slide::Custom);
    setupDestChooser(slide->slideType());

    if(custom) {
        mDestXChooser->setEditText(slide->destX());
        mDestYChooser->setEditText(slide->destY());
    }
    else {
        mDestXChooser->setCurrentIndex(mDestXChooser->findData(slide->destX()));
        mDestYChooser->setCurrentIndex(mDestYChooser->findData(slide->destY()));
    }

    mAction = action;
}

void SlideEditorWidget::onButtonClicked()
{
    QPushButton* btn = static_cast<QPushButton*>(sender());
    btn->setChecked(! btn->isChecked());
    //mIsDown = true;
    //btn->setDown(true);
}

bool SlideEditorWidget::eventFilter(QObject *obj, QEvent *event)
{
    /*if (event->type() == QEvent::MouseButtonRelease) {
        if (mIsDown)
            return true;
    }
    else if (event->type() == QEvent::MouseButtonPress) {

        if (obj == mStartButton) {
            mStartButton->setChecked(! mStartButton->isChecked());
            //mStartButton->setDown(! mStartButton->isDown());
            //mIsDown = mStartButton->isDown();
            return true;
        }
        else if (obj == mEndButton) {
            mEndButton->setDown(! mEndButton->isDown());
            mIsDown = mEndButton->isDown();
            return true;
        }
    }*/

    return false;
    //if ((obj == mStartButton || obj == mEndButton) && event->)
}

void SlideEditorWidget::onDurationChanged(double dur) {
    Slide * slide  = qobject_cast<Slide*>(mAction);
    if (slide)
        slide->setDuration(dur);
}

void SlideEditorWidget::onResourceChanged(int index)
{
    Slide * slide  = qobject_cast<Slide*>(mAction);
    if (! slide)
        return;

    if (index >= 0 && index < objects().size()) {
        Object* obj = objects()[index];

        if (slide->sceneObject() != obj) {
            slide->setSceneObject(obj);
        }
    }
}

void SlideEditorWidget::onTypeChanged(int type)
{
    Slide * slide  = qobject_cast<Slide*>(mAction);
    if (slide) {
        slide->setSlideType(static_cast<Slide::Type>(type));
        setupDestChooser(slide->slideType());
    }
}

void SlideEditorWidget::onDestXChanged(const QString & pos)
{
    Slide * slide  = qobject_cast<Slide*>(mAction);
    if (slide)
        slide->setDestX(pos.toInt());
}

void SlideEditorWidget::onDestYChanged(const QString & pos)
{
    Slide * slide  = qobject_cast<Slide*>(mAction);
    if (slide)
        slide->setDestY(pos.toInt());
}

void SlideEditorWidget::onDestXChanged(int index)
{
    Slide * slide  = qobject_cast<Slide*>(mAction);
    if (slide)
        slide->setDestX(mDestXChooser->itemData(index).toString());
}

void SlideEditorWidget::onDestYChanged(int index)
{
    Slide * slide  = qobject_cast<Slide*>(mAction);
    if (slide)
        slide->setDestY(mDestYChooser->itemData(index).toString());
}

void SlideEditorWidget::setupDestChooser(int type)
{
    bool custom = (type == Slide::Custom);
    mDestXChooser->setEditable(custom);
    mDestYChooser->setEditable(custom);
    mDestXChooser->clear();
    mDestYChooser->clear();

    if (type == Slide::Custom) {
        mDestXChooser->lineEdit()->setPlaceholderText("X");
        mDestYChooser->lineEdit()->setPlaceholderText("Y");
    }
    else if (type == Slide::Out || type == Slide::In) {
        mDestXChooser->addItem("", "");
        mDestXChooser->addItem(tr("Left"), "left");
        if (type == Slide::In)
            mDestXChooser->addItem(tr("Center"), "center");
        mDestXChooser->addItem(tr("Right"), "right");
        mDestYChooser->addItem("", "");
        mDestYChooser->addItem(tr("Top"), "top");
        if (type == Slide::In)
            mDestYChooser->addItem(tr("Center"), "center");
        mDestYChooser->addItem(tr("Bottom"), "bottom");
        mDestXChooser->setCurrentIndex(0);
        mDestYChooser->setCurrentIndex(0);
    }
}
