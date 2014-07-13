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

#include "combobox.h"

#include <QListView>
#include <QStandardItemModel>
#include <QStyleOptionViewItemV4>
#include <QDebug>
#include <QApplication>
#include <QEvent>
#include <QMouseEvent>
#include <QVariant>

ComboBoxItemDelegate::ComboBoxItemDelegate(QObject *parent):
    QStyledItemDelegate(parent)
{
    mDeleteIcon = QIcon(":/media/delete.png");
}

void ComboBoxItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(index.model());
    if (! model)
        return;

    if (index.row() == model->rowCount()-1) {
        QStyledItemDelegate::paint(painter, option, index);
    }
    else { //Add remove icon

        int height = option.fontMetrics.height();
        //QStyleOptionViewItemV4 optionText(option);
        //optionText.rect.setWidth(optionText.rect.width()-height);
        QStyledItemDelegate::paint(painter, option, index);
        if (index.data(Qt::UserRole) != "NOT_VALID") {
            QStyleOptionViewItemV4 optionIcon(option);
            optionIcon.rect.setX(option.rect.width()-height);
            optionIcon.rect.setWidth(height);
            //QStyle *style = QApplication::style();
            //style->drawControl(QStyle::CE_ItemViewItem, &optionIcon, painter);
            mDeleteIcon.paint(painter, optionIcon.rect);
        }

    }
    //QStyledItemDelegate::paint(painter, option, index);
}

bool ComboBoxItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    const QStandardItemModel* itemModel = qobject_cast<const QStandardItemModel*>(model);
    if (! itemModel || index.data(Qt::UserRole) == "NOT_VALID")
        return QStyledItemDelegate::editorEvent(event, model, option, index);


    if (event->type() == QEvent::MouseButtonPress && index.row() < itemModel->rowCount()-1) {
        QMouseEvent *ev = static_cast<QMouseEvent*>(event);
        int height = option.rect.height();
        if (ev->x() >= option.rect.width()-height) {
            emit removeItem(index.row());
            return true;
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

ComboBox::ComboBox(QWidget *parent, const QString& defaultValue) :
    QComboBox(parent)
{
    mHidePopup = true;
    mDialog = 0;
    if (! defaultValue.isEmpty())
        mDefaultValue = defaultValue;
    else
        mDefaultValue = tr("None");
    clear();
    if (view()->itemDelegate())
        view()->itemDelegate()->deleteLater();
    ComboBoxItemDelegate * itemDelegate = new ComboBoxItemDelegate(view());
    this->view()->setItemDelegate(itemDelegate);
    this->view()->installEventFilter(this);
    connect(itemDelegate, SIGNAL(removeItem(int)), this, SLOT(onRemoveItem(int)));
    connect(this, SIGNAL(activated(int)), this, SLOT(onItemActivated(int)));
}

void ComboBox::addItem(const QIcon &icon, const QString &text, const QVariant &userData)
{
    switch(count())
    {
    case 0:
        insertItem(0, icon, text, userData);
        break;
    case 1:

    case 2:
        if (itemData(0).toString() == "NOT_VALID")
            removeItem(0);
    default:
        insertItem(count()-1, icon, text, userData);
        setCurrentIndex(count()-2);
    }

}

void ComboBox::addItem(const QString &text, const QVariant &userData)
{
    addItem(QIcon(), text, userData);
}

void ComboBox::removeItem(int index)
{
    if (index == count()-1)
        return;

    QComboBox::removeItem(index);
}

bool ComboBox::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == this->view() && ev->type() == QEvent::Hide){
        if (! mHidePopup) {
            this->showPopup();
            mHidePopup = true;
            return true;
        }
    }

    if (obj == this->view() && ev->type() == QEvent::MouseButtonRelease){
        return true;
    }

    return QObject::eventFilter(obj, ev);
}

void ComboBox::onRemoveItem(int index)
{
    if (index < 0)
        return;

    emit itemRemoved(index);
    emit itemRemoved(itemText(index));

    /*QVariant data = itemData(index);
    QObject* obj = data.value<QObject*>();
    if (obj)
        obj->deleteLater();*/

    removeItem(index);
    if (count() == 1) {
        insertItem(0, tr("None"), QString("NOT_VALID"));
    }
    mHidePopup = false;
}

void ComboBox::setDialog(QDialog * dialog)
{
    if (! dialog)
        return;

    if (mDialog)
        mDialog->deleteLater();

    mDialog = dialog;
    mDialog->setParent(this);
}

void ComboBox::setDefaultValue(const QString & text)
{
    mDefaultValue = text;
}

void ComboBox::onItemActivated(int index)
{
    if (index == count()-1) {
        setCurrentIndex(0);
        emit addItemActivated();
    }
    else {
        emit itemActivated(index);
    }

    /*if (! mDialog)
        return;

    if (index == count()-1) {
        mDialog->exec();
    }

    setCurrentIndex(0);
    */
}

void ComboBox::wheelEvent(QWheelEvent * e)
{
    //No wheel event
}

void ComboBox::clear()
{
   QComboBox::clear();
   QComboBox::addItem(mDefaultValue, QString("NOT_VALID"));
   QComboBox::addItem(QIcon(":/media/add.png"), tr("Add More"));
   setCurrentIndex(0);
}
