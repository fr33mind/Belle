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
#include <QPainter>
#include <QPushButton>

ComboBoxItemDelegate::ComboBoxItemDelegate(QObject *parent, QComboBox* cmb):
    QStyledItemDelegate(parent)
{
    mDeleteIcon = QIcon(":/media/delete.png");
    mCombo = cmb;
}

void ComboBoxItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    if (isSeparator(index))
        return;

    if (index.data(Qt::UserRole).toString() != INVALID_DATA) {
        int height = option.fontMetrics.height();
        QStyleOptionViewItemV4 optionIcon(option);
        optionIcon.rect.setX(option.rect.width()-height);
        optionIcon.rect.setWidth(height);
        bool showDelIcon = true;
        QVariant removable = index.data(ITEM_REMOVABLE_ROLE);
        if (removable.type() == QVariant::Bool)
            showDelIcon = removable.toBool();
        if (showDelIcon)
            mDeleteIcon.paint(painter, optionIcon.rect);
    }
}

bool ComboBoxItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    //FIXME: Figure out why MouseButtonRelease doesn't show up here.
    if (event->type() == QEvent::MouseButtonPress) {
        mLastStyleOption = option;
        mLastIndex = index;
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

QSize ComboBoxItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

bool ComboBoxItemDelegate::mouseReleased(QEvent * event)
{
    if (event->type() != QEvent::MouseButtonRelease)
        return false;

    QAbstractItemModel* model = mCombo->model();
    QMouseEvent *ev = static_cast<QMouseEvent*>(event);
    QModelIndex index = mCombo->view()->indexAt(ev->pos());
    QVariant removable = index.data(ITEM_REMOVABLE_ROLE);
    bool remove = true;
    if (removable.type() == QVariant::Bool)
        remove = removable.toBool();

    if (remove && mLastIndex == index && model && index.data(Qt::UserRole).toString() != INVALID_DATA) {
        QStyleOptionViewItem opt = mLastStyleOption;
        int height = opt.rect.height();
        if (ev->x() >= opt.rect.width()-height) {
            emit removeItem(index.row());
            return true;
        }
    }

    return false;
}

ComboBox::ComboBox(QWidget *parent, const QString& defaultText) :
    QComboBox(parent)
{
    if (! defaultText.isEmpty())
        mDefaultText = defaultText;
    else
        mDefaultText = tr("None");
    clear();
    if (itemDelegate())
        itemDelegate()->deleteLater();
    mItemDelegate = new ComboBoxItemDelegate(this, this);
    this->setItemDelegate(mItemDelegate);
    connect(mItemDelegate, SIGNAL(removeItem(int)), this, SLOT(onRemoveItem(int)));
    this->view()->viewport()->installEventFilter(this);
    connect(this, SIGNAL(activated(int)), this, SLOT(onItemActivated(int)));
}

void ComboBox::addItem(const QIcon &icon, const QString &text, const QVariant &userData)
{
    switch(count())
    {
    case 0:
        clear();
    case 1:
    case 2:
        if (itemData(0).toString() == INVALID_DATA)
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
    if (ev->type() == QEvent::MouseButtonRelease && obj == view()->viewport()){
         if (mItemDelegate->mouseReleased(ev))
            return true;
    }

    return QComboBox::eventFilter(obj, ev);
}

void ComboBox::onRemoveItem(int index)
{
    if (index < 0)
        return;

    QString text = itemText(index);

    removeItem(index);

    emit itemRemoved(index);
    emit itemRemoved(text);

    if (count() == 1)
        clear();

    if (currentIndex() == count()-1)
        setCurrentIndex(count()-2);

    showPopup();
}

void ComboBox::setDefaultText(const QString & text)
{
    mDefaultText = text;
}

QString ComboBox::defaultText() const
{
    return mDefaultText;
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
}

void ComboBox::wheelEvent(QWheelEvent * e)
{
    //No wheel event
}

void ComboBox::clear()
{
   QComboBox::clear();
   QComboBox::addItem(mDefaultText, INVALID_DATA);
   QComboBox::addItem(QIcon(":/media/add.png"), tr("Add More"), INVALID_DATA);
   setCurrentIndex(0);
}

void ComboBox::setItemRemovable(int index, bool removable)
{
    setItemData(index, removable, ITEM_REMOVABLE_ROLE);
}
