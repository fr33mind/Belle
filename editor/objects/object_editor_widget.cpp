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

#include "object_editor_widget.h"

#include <QPushButton>
#include <QSlider>
#include <QColorDialog>
#include <QDebug>
#include <QLineEdit>
#include <QVariant>
#include <QFileDialog>

#include "gotolabel.h"
#include "color_pushbutton.h"
#include "combobox.h"
#include "add_action_dialog.h"
#include "scene.h"
#include "drawing_surface_widget.h"

QHash<QObject*, Interaction::InputEvent> mWidgetToEvent = QHash<QObject*, Interaction::InputEvent>();

ObjectEditorWidget::ObjectEditorWidget(QWidget *parent) :
    GameObjectEditorWidget(parent)
{
    QStandardItem* lastItem = 0;
    QRegExpValidator * validator = new QRegExpValidator(QRegExp("^[0-9]+(%){0,1}$"), this);

    //mChooseObjectComboBox = new QComboBox(this);
    //mChooseObjectComboBox = 0;
    mVisibleCheckbox = new QCheckBox(this);
    mXSpin = new QSpinBox(this);
    mYSpin = new QSpinBox(this);
    mWidthEditor = new QLineEdit(this);
    mWidthEditor->setValidator(validator);
    mWidthEditor->setObjectName("widthEditor");
    mHeightEditor = new QLineEdit(this);
    mHeightEditor->setValidator(validator);
    mHeightEditor->setObjectName("heightEditor");
    mKeepAspectRatioCheckbox = new QCheckBox(this);
    mOpacitySlider = new QSlider(Qt::Horizontal, this);
    mOpacitySlider->setMaximum(255);

    connect(mVisibleCheckbox, SIGNAL(toggled(bool)), this, SLOT(onVisibilityChanged(bool)));
    connect(mXSpin, SIGNAL(valueChanged(int)), this, SLOT(onXChanged(int)));
    connect(mYSpin, SIGNAL(valueChanged(int)), this, SLOT(onYChanged(int)));
    connect(mWidthEditor, SIGNAL(textEdited(const QString &)), this, SLOT(onSizeEdited(const QString&)));
    connect(mHeightEditor, SIGNAL(textEdited(const QString &)), this, SLOT(onSizeEdited(const QString&)));
    connect(mKeepAspectRatioCheckbox, SIGNAL(toggled(bool)), this, SLOT(onKeepAspectRatioToggled(bool)));
    connect(mOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(onOpacityChanged(int)));


    this->beginGroup(tr("Object"), "Object");
    //this->appendRow(tr("Current"), mChooseObjectComboBox);
    this->appendRow(tr("Visible"), mVisibleCheckbox);
    this->appendRow("x", mXSpin);
    this->appendRow("y", mYSpin);
    this->appendRow(tr("Width"), mWidthEditor);
    this->appendRow(tr("Height"), mHeightEditor);
    this->appendRow(tr("Keep aspect ratio"), mKeepAspectRatioCheckbox);
    this->appendRow(tr("Opacity"), mOpacitySlider);
    this->endGroup();

    mResourceLabel = new QLabel(this);
    mResourceLabel->setMargin(4);
    mSyncCheckbox = new QCheckBox(this);
    connect(mSyncCheckbox, SIGNAL(toggled(bool)), this, SLOT(syncToggled(bool)));

    this->beginGroup(tr("Resource"), "Resource");
    lastItem = this->lastItem();
    this->appendRow(tr("Name"), mResourceLabel);
    this->appendRow(tr("Sync"), mSyncCheckbox);
    this->endGroup();
    lastItem->child(1)->setToolTip(tr("Synchronizes data from this object to the resource "
                                      "it was cloned from."));

    mClonesComboBox = new QComboBox(this);

    this->beginGroup(tr("Clones"), "Clones");
    this->appendRow(tr("Clones"), mClonesComboBox);
    this->endGroup();

    this->beginGroup(tr("Bounding Rect"));
    mBorderWidthSpinBox = new QSpinBox(this);
    mBorderWidthSpinBox->setMaximum(50);
    mBorderColorButton = new ColorPushButton(this);
    mCornerRadiusSpinBox = new QSpinBox(this);

    connect(mBorderWidthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onBorderWidthChanged(int)));
    connect(mBorderColorButton, SIGNAL(colorChosen(const QColor&)), this, SLOT(onBorderColorChanged(const QColor&)));
    connect(mCornerRadiusSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onCornerRadiusValueChanged(int)));

    appendRow(tr("Border Width"), mBorderWidthSpinBox);
    appendRow(tr("Border Color"), mBorderColorButton);
    appendRow(tr("Corner Radius"), mCornerRadiusSpinBox);
    endGroup();

    mImageChooser = new ChooseFileButton(ChooseFileButton::ImageFilter, this);
    mColorButton = new ColorPushButton(this);
    mBackgroundOpacitySlider = new QSlider(Qt::Horizontal, this);
    mBackgroundOpacitySlider->setMaximum(255);

    connect(mImageChooser, SIGNAL(fileSelected(const QString&)), this, SLOT(onImageChosen(const QString&)));
    connect(mBackgroundOpacitySlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
    connect(mColorButton, SIGNAL(colorChosen(const QColor&)), this, SLOT(onColorChosen(const QColor&)));

    this->beginGroup(tr("Background"));
    this->appendRow(tr("Color"), mColorButton);
    this->appendRow(tr("Image"), mImageChooser);
    this->appendRow(tr("Opacity"), mBackgroundOpacitySlider);
    this->endGroup();

    this->beginGroup(tr("Events"));
    mMousePressComboBox = new ComboBox(this);
    mMousePressComboBox->setObjectName("MousePressComboBox");
    connect (mMousePressComboBox, SIGNAL(itemActivated(int)), this, SLOT(onItemActivated(int)));
    connect (mMousePressComboBox, SIGNAL(addItemActivated()), this, SLOT(onAddItemActivated()));
    connect (mMousePressComboBox, SIGNAL(itemRemoved(int)), this, SLOT(onEventItemRemoved(int)));
    this->appendRow("OnMousePress", mMousePressComboBox);
    mWidgetToEvent.insert(mMousePressComboBox, Interaction::MousePress);

    mMouseReleaseComboBox = new ComboBox(this);
    mMouseReleaseComboBox->setObjectName("MouseReleaseComboBox");
    connect (mMouseReleaseComboBox, SIGNAL(itemActivated(int)), this, SLOT(onItemActivated(int)));
    connect (mMouseReleaseComboBox, SIGNAL(addItemActivated()), this, SLOT(onAddItemActivated()));
    connect (mMouseReleaseComboBox, SIGNAL(itemRemoved(int)), this, SLOT(onEventItemRemoved(int)));
    this->appendRow("OnMouseRelease", mMouseReleaseComboBox);
    mWidgetToEvent.insert(mMouseReleaseComboBox, Interaction::MouseRelease);
    this->endGroup();

    mMouseMoveComboBox = new ComboBox(this);
    mMouseMoveComboBox->setObjectName("MouseMoveComboBox");
    connect (mMouseMoveComboBox, SIGNAL(itemActivated(int)), this, SLOT(onItemActivated(int)));
    connect (mMouseMoveComboBox, SIGNAL(addItemActivated()), this, SLOT(onAddItemActivated()));
    connect (mMouseMoveComboBox, SIGNAL(itemRemoved(int)), this, SLOT(onEventItemRemoved(int)));
    this->appendRow("OnMouseMove", mMouseMoveComboBox);
    mWidgetToEvent.insert(mMouseMoveComboBox, Interaction::MouseMove);
    this->endGroup();

    resizeColumnToContents(0);
}

void ObjectEditorWidget::onXChanged(int x)
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (object)
        object->setX(x);
}

void ObjectEditorWidget::onYChanged(int y)
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (object)
        object->setY(y);
}

void ObjectEditorWidget::onCornerRadiusValueChanged(int v)
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (object)
        object->setCornerRadius(v);
}

void ObjectEditorWidget::onSliderValueChanged(int value)
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (object)
        object->setBackgroundOpacity(value);
}

void ObjectEditorWidget::onOpacityChanged(int value)
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (object)
        object->setOpacity(value);
}

void ObjectEditorWidget::onColorChosen(const QColor & color)
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (object)
        object->setBackgroundColor(color);
}

void ObjectEditorWidget::updateData(GameObject* obj)
{
    GameObjectEditorWidget::updateData(obj);
    Object* currObj = qobject_cast<Object*>(obj);
    if (! currObj)
        return;

    connect(currObj, SIGNAL(dataChanged(const QVariantMap&)), this, SLOT(onObjectDataChanged(const QVariantMap&)));
    connect(currObj, SIGNAL(synced()), this, SLOT(reload()));

    if (currObj->resource()) {
       this->setFilters(QStringList() << "Clones");
       mResourceLabel->setText(currObj->resource()->name());
       mSyncCheckbox->setChecked(currObj->isSynced());
    }
    else if (currObj->isResource()) {
        this->setFilters(QStringList() << "Resource");
        QList<GameObject*> clones = currObj->clones();
        QString name("");
        mClonesComboBox->clear();

        for(int i=0; i < clones.size(); i++) {
            name = QString("\"%1\" in \"%2\" (%3)").arg(clones[i]->name())
                                     .arg(clones[i]->scene() ? clones[i]->scene()->objectName() : "")
                                     .arg(clones[i]->isSynced() ? tr("Synced") : tr("Not synced"));
            mClonesComboBox->addItem(name);
        }
    }
    else {
        this->setFilters(QStringList() << "Resource" << "Clones");
    }

    ////mChooseObjectComboBox->clear();
    mObjectsHierarchy.clear();

    if (currObj->resource()) {
        //mChooseObjectComboBox->addItem(currObj->resource()->objectName() + tr("(resource)"));
        //mObjectsHierarchy.append(currObj->resource());
    }

    //mChooseObjectComboBox->addItem(currObj->objectName() + tr("(copy)"));
    mObjectsHierarchy.append(currObj);
    mOpacitySlider->setValue(currObj->opacity());
    mBorderWidthSpinBox->setValue(currObj->borderWidth());
    mBorderColorButton->setColor(currObj->borderColor());
    mColorButton->setText(currObj->backgroundColor().name());
    mColorButton->setColor(currObj->backgroundColor());
    mImageChooser->setImageFile(currObj->backgroundImage());
    mBackgroundOpacitySlider->setValue(currObj->backgroundOpacity());
    mXSpin->setRange(-currObj->width(), Scene::width());
    mXSpin->setValue(currObj->x());
    mYSpin->setRange(-currObj->height(), Scene::height());
    mYSpin->setValue(currObj->y());
    if (currObj->percentWidth())
        mWidthEditor->setText(QString::number(currObj->percentWidth()));
    else
        mWidthEditor->setText(QString::number(currObj->width()));
    if (currObj->percentWidth())
        mHeightEditor->setText(QString::number(currObj->percentHeight()));
    else
        mHeightEditor->setText(QString::number(currObj->height()));

    updateEventActions(currObj);

    mCornerRadiusSpinBox->setValue(currObj->cornerRadius());
    mVisibleCheckbox->setChecked(currObj->visible());
    mKeepAspectRatioCheckbox->setChecked(currObj->keepAspectRatio());
}

void ObjectEditorWidget::updateEventActions(Object* object)
{
    QList<Action*> actions;
    Action *action;

    mMousePressComboBox->clear();
    mMouseReleaseComboBox->clear();
    mMouseMoveComboBox->clear();

    if (! object)
        return;

    const GameObjectMetaType* metatype = GameObjectMetaType::metaType(action->type());
    const QIcon typeIcon = metatype ? metatype->icon() : QIcon();

    actions = object->actionsForEvent(Interaction::MousePress);
    for(int i=0; i < actions.size(); i++) {
        action = actions[i];
        mMousePressComboBox->addItem(typeIcon, action->toString(), QVariant::fromValue(qobject_cast<QObject*>(action)));
    }

    actions = object->actionsForEvent(Interaction::MouseRelease);
    for(int i=0; i < actions.size(); i++) {
        action = actions[i];
        mMouseReleaseComboBox->addItem(typeIcon, action->toString(), QVariant::fromValue(qobject_cast<QObject*>(action)));
    }

    actions = object->actionsForEvent(Interaction::MouseMove);
    for(int i=0; i < actions.size(); i++) {
        action = actions[i];
        mMouseMoveComboBox->addItem(typeIcon, action->toString(), QVariant::fromValue(qobject_cast<QObject*>(action)));
    }
}

void ObjectEditorWidget::onAddItemActivated()
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (! object)
        return;

    Interaction::InputEvent event = mWidgetToEvent.value(sender());
    AddActionDialog dialog(object);
    dialog.exec();

    if (dialog.result() == QDialog::Accepted && dialog.selectedAction()) {
        Action* action = dialog.selectedAction();
        object->appendEventAction(event, action);
        ComboBox *comboBox = qobject_cast<ComboBox*>(sender());
        if(comboBox) {
            const GameObjectMetaType* metatype = GameObjectMetaType::metaType(action->type());
            const QIcon typeIcon = metatype ? metatype->icon() : QIcon();
            comboBox->addItem(typeIcon, action->toString(), QVariant::fromValue(static_cast<QObject*>(action)));
        }
    }
}

void ObjectEditorWidget::onItemActivated(int index)
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (! object)
        return;

    Interaction::InputEvent event = mWidgetToEvent.value(sender());
    QList<Action*> actions = object->actionsForEvent(event);
    if (index >= actions.size())
        return;

    Action * action = actions[index];
    AddActionDialog dialog(action);
    dialog.exec();

    ComboBox *comboBox = qobject_cast<ComboBox*>(sender());
    if(comboBox) {
        comboBox->setItemText(index, action->toString());
    }
}

void ObjectEditorWidget::onSizeEdited(const QString & text)
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (! object)
        return;

    bool ok;
    int size = 0;
    QString value(text);
    bool percent = (text.indexOf("%") != -1);

    if (percent)
        value.remove("%");

    size = value.toInt(&ok);

    if (ok) {
        if (sender()->objectName().contains("width"))
            object->setWidth(size, percent);
        else
            object->setHeight(size, percent);
    }
}

/*bool ObjectEditorWidget::eventFilter(QObject * obj, QEvent *event)
{
    if (obj == mImagePathEdit && event->type() == QEvent::MouseButtonDblClick) {
        QString filter = tr("Images(*.png *.xpm *.jpg *.gif)");
        QString path = QFileDialog::getOpenFileName(this, tr("Choose Background Image"), QDir::currentPath(), filter);
        if (mCurrentObject && ! path.isEmpty()) {
            mCurrentObject->setBackgroundImage(path);
            mImagePathEdit->setText(path);
            mImagePathEdit->setToolTip(QString("<img src=%1></img>").arg(path));
        }
        return true;
    }

    return PropertiesWidget::eventFilter(obj, event);
}*/

void ObjectEditorWidget::onImageChosen(const QString & filepath)
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (object)
        object->setBackgroundImage(filepath);
}

void ObjectEditorWidget::onVisibilityChanged(bool visible)
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (object)
        object->setVisible(visible);
}

void ObjectEditorWidget::onEventItemRemoved(int index)
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (! object || index < 0)
        return;

    QString name = sender()->objectName();

    if (name == "MousePressComboBox") {
        object->removeEventActionAt(Interaction::MousePress, index, true);
    }
    else if (name == "MouseReleaseComboBox") {
        object->removeEventActionAt(Interaction::MouseRelease, index, true);
    }
    else if (name == "MouseMoveComboBox") {
        object->removeEventActionAt(Interaction::MouseMove, index, true);
    }
}

void ObjectEditorWidget::onBorderWidthChanged(int w)
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (object)
        object->setBorderWidth(w);
}

void ObjectEditorWidget::onBorderColorChanged(const QColor & color)
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (object)
        object->setBorderColor(color);
}

void ObjectEditorWidget::onObjectDataChanged(const QVariantMap & data)
{
    int w=-1, h=-1;

    mXSpin->blockSignals(true);
    mYSpin->blockSignals(true);

    if (data.contains("x") && data.value("x").canConvert(QVariant::Int))
        mXSpin->setValue(data.value("x").toInt());
    if (data.contains("y") && data.value("y").canConvert(QVariant::Int))
        mYSpin->setValue(data.value("y").toInt());
    if (data.contains("width") && data.value("width").canConvert(QVariant::String))
        w = data.value("width").toInt();
    if (data.contains("height") && data.value("height").canConvert(QVariant::String))
        h = data.value("height").toInt();

    setObjectSize(w, h);

    mXSpin->blockSignals(false);
    mYSpin->blockSignals(false);
}

void ObjectEditorWidget::setObjectSize(int w, int h)
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (! object)
        return;

    mWidthEditor->blockSignals(true);
    mHeightEditor->blockSignals(true);

    if (w != -1) {
        if(object->percentWidth())
            mWidthEditor->setText(QString::number(object->percentWidth())+"%");
        else
            mWidthEditor->setText(QString::number(w));
    }

    if (h != -1) {
        if(object->percentHeight())
            mHeightEditor->setText(QString::number(object->percentHeight())+"%");
        else
            mHeightEditor->setText(QString::number(h));
    }

    mWidthEditor->blockSignals(false);
    mHeightEditor->blockSignals(false);
}

void ObjectEditorWidget::onKeepAspectRatioToggled(bool keep)
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (object)
        object->setKeepAspectRatio(keep);
}

void ObjectEditorWidget::syncToggled(bool _sync)
{
    Object* object = qobject_cast<Object*>(mGameObject);
    if (object)
        object->setSync(_sync);
}
