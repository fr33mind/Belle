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

#include "properties_widget.h"

#include <QDebug>
#include <QHeaderView>
#include <QAbstractItemModel>

PropertiesWidget::PropertiesWidget(QWidget *parent, int columns) :
    QTreeView(parent)
{
    //this->setFrameStyle(QFrame::NoFrame);
    mModel = new QStandardItemModel(this);
    //QPalette pal = palette();
    //pal.setColor(QPalette::Base, pal.color(QPalette::Window));
    //this->setPalette(pal);
    this->setModel(mModel);
    mModel->setColumnCount(columns);
    //this->setHeaderHidden(true);
    mModel->setHeaderData(0, Qt::Horizontal, tr("Property"));
    mModel->setHeaderData(1, Qt::Horizontal, tr("Value"));

    setFocusPolicy(Qt::NoFocus);
    mLastItem = 0;
    //this->header()->setResizeMode(QHeaderView::ResizeToContents);
}

void PropertiesWidget::beginGroup(const QString & name, const QString & key)
{
    QStandardItem *root = new QStandardItem(name);
    root->setEditable(false);
    if (! key.isEmpty())
        root->setData(key);
    mModel->appendRow(QList<QStandardItem*>() << root);
    mLastItem = root;
}

void PropertiesWidget::appendRow(QStandardItem * item)
{
    mModel->appendRow(item);
}

void PropertiesWidget::appendRow(const QString& text, const QString& text2, const QString & key)
{
    if (! mLastItem)
        return;

    QStandardItem *item = mLastItem;
    QStandardItem *child = new QStandardItem(text);
    QStandardItem *child2 = new QStandardItem(text2);
    child->setEditable(false);
    if (! key.isEmpty())
        child->setData(key);
    item->appendRow(QList<QStandardItem*>() << child << child2);
    this->expand(child->index().parent());

}

void PropertiesWidget::appendRow(const QIcon& icon, const QString& text, const QString & key)
{
    if (! mLastItem)
        return;

    QStandardItem *item = mLastItem;
    QStandardItem *child = new QStandardItem(text);
    child->setIcon(icon);
    child->setEditable(false);
    if (! key.isEmpty())
        child->setData(key);
    item->appendRow(QList<QStandardItem*>() << child);
    this->expand(child->index().parent());
}


void PropertiesWidget::appendRow(const QString& text, QWidget* widget, const QString & key)
{
    if (! mLastItem)
        return;

    QStandardItem *item = mLastItem;
    QStandardItem *child = new QStandardItem(text);
    QStandardItem *child2 = new QStandardItem;
    child->setEditable(false);
    if (! key.isEmpty())
        child->setData(key);
    item->appendRow(QList<QStandardItem*>() << child << child2);
    if (widget)
        this->setIndexWidget(child2->index(), widget);
    this->expand(child->index().parent());
}

void PropertiesWidget::appendRow(PropertiesWidget* propertiesWidget, QStandardItem * itemLeft, QStandardItem * itemRight)
{
    if (! itemLeft)
        return;

    QWidget* widget = 0;
    QString text("");

    if (itemRight) {
         widget = propertiesWidget->indexWidget(itemRight->index());
         if (! widget)
             text = itemRight->data(Qt::DisplayRole).toString();
    }

    if (widget)
        appendRow(itemLeft->data(Qt::DisplayRole).toString(), widget);
    else if (! text.isEmpty())
        appendRow(itemLeft->data(Qt::DisplayRole).toString(), text);
    else
        appendRow(itemLeft->data(Qt::DisplayRole).toString(), 0);
}

void PropertiesWidget::endGroup()
{
    /*int lastRow = mModel->rowCount()-1;
    if (lastRow < 0)
        lastRow = 0;
    mLastItem = mModel->item(lastRow, 0);*/

    if (mLastItem && mLastItem->parent())
        mLastItem = mLastItem->parent();
}

void PropertiesWidget::drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const
{
    QStyleOptionViewItemV4 options2(options);

    QColor color(121, 116, 113);
    options2.palette.setColor(QPalette::Base, color);

    if (! index.parent().isValid()) {
        painter->fillRect(options.rect, color);

        QColor color2;
        color2.setRgb(255, 255, 255);
        options2.palette.setColor(QPalette::Text, color2);
        QFont font = this->font();
        font.setBold(true);
        options2.font = font;
    }

    QTreeView::drawRow(painter, options2, index);
}

/*bool PropertiesWidget::edit (const QModelIndex & index, EditTrigger trigger, QEvent * event)
{
    if (index.column() > 0 && index.parent().isValid()) {
        return QTreeView::edit(index, trigger, event);
    }

    return QTreeView::edit(index, trigger, event);;
}*/

void PropertiesWidget::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if (topLeft.column() == 0)
        return;

    int row = topLeft.row();
    QString value = topLeft.data().toString();
    QString key = topLeft.sibling(row, 0).data().toString();
    emit dataChanged(key, value);
}

void PropertiesWidget::setData(const QString& key, const QString& value)
{
    QModelIndex index = QModelIndex();

    for(int i=0; i < mModel->rowCount(); i++) {
        index = mModel->index(i, 0);
        if (! index.parent().isValid())
            continue;

        QString data = mModel->index(i, 0).data().toString();
        if (data == key) {
            mModel->setData(mModel->index(i, 1), value);
            break;
        }
    }
}

void PropertiesWidget::beginSubGroup(const QString & name, const QString& text)
{
    if (! mLastItem)
        return;

    appendRow(name, text);
    updateLastItem();
}

void PropertiesWidget::beginSubGroup(const QString & name, QWidget* widget)
{
    if (! mLastItem)
        return;

    appendRow(name, widget);
    updateLastItem();
}

void PropertiesWidget::beginSubGroup(PropertiesWidget* propertiesWidget, QStandardItem* itemLeft, QStandardItem* itemRight)
{
    if (! mLastItem || ! itemLeft)
        return;

    appendRow(propertiesWidget, itemLeft, itemRight);
    updateLastItem();
}

bool PropertiesWidget::containsGroup(const QString& name)
{

    QStandardItem* item = 0;
    QStandardItem* child = 0;

    for(int i=0; i < mModel->rowCount(); i++) {
        item = mModel->item(i);
        if (item && item->text() == name)
            return true;

        for(int j=0; j < item->rowCount(); j++) {
            child = item->child(j);
            if (child && child->text() == name)
                return true;
        }
    }

    return false;
}


void PropertiesWidget::setLastItem(const QString& name)
{

    QStandardItem* item = 0;
    QStandardItem* child = 0;
    QStandardItem* groupItem = 0;

    for(int i=0; i < mModel->rowCount(); i++) {
        item = mModel->item(i);
        if (item && item->text() == name) {
            groupItem = item;
            break;
        }

        for(int j=0; j < item->rowCount(); j++) {
            child = item->child(j);
            if (child && child->text() == name) {
                groupItem = child;
            }
        }

        if (groupItem)
            break;
    }

    if (groupItem)
        mLastItem = groupItem;

}

QStandardItem* PropertiesWidget::lastItem() const
{
    return mLastItem;
}

void PropertiesWidget::setLastItem(QStandardItem * item)
{
    mLastItem = item;
}

void PropertiesWidget::updateLastItem()
{
    if (! mLastItem)
        return;

    QStandardItem* item = mLastItem;
    if (item->hasChildren())
        item = item->child(item->rowCount()-1);

    mLastItem = item;
}

void PropertiesWidget::setGroupName(const QString& name)
{
    if (mLastItem)
        mLastItem->setData(name, Qt::DisplayRole);
}

void PropertiesWidget::append(PropertiesWidget* propertiesWidget, int startAt, const QStringList& filters)
{
    if (! propertiesWidget)
        return;

    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(propertiesWidget->model());
    if (! model)
        return;

    QStandardItem *item;
    QStandardItem* lastItemBackup = mLastItem;
    mLastItem = mModel->item(mModel->rowCount()-1);

    for(int i=startAt; i < model->rowCount(); i++) {
        item = model->item(i);

        if (filters.contains(item->data().toString()))
            continue;

        beginGroup(item->data(Qt::DisplayRole).toString());

        if (item->hasChildren()) {
            bool finishedAllItems;
            QList<int> positions;
            positions.append(0);
            int nestingLevel = 0;

            while (true) {
                finishedAllItems = true;

                for(int j=positions[nestingLevel]; j < item->rowCount(); j++) {
                    if (item->child(j)->hasChildren()) {

                        if (filters.contains(item->child(j)->data().toString()))
                            continue;

                        finishedAllItems = false;
                        beginSubGroup(propertiesWidget, item->child(j), item->child(j, 1));

                        nestingLevel < positions.size() ? positions.replace(nestingLevel, j+1) : positions.append(j+1);
                        nestingLevel++;
                        nestingLevel < positions.size() ? positions.replace(nestingLevel, 0) : positions.append(0);

                        item = item->child(j);
                        break;
                    }

                    if (! filters.contains(item->child(j)->data().toString()))
                        appendRow(propertiesWidget, item->child(j), item->child(j, 1));
                }

                if (nestingLevel == 0)
                    break;

                if (finishedAllItems && nestingLevel > 0) {
                    nestingLevel--;
                    item = item->parent();
                    endGroup();
                }
            }
        }

        endGroup();
    }

    endGroup();

    mLastItem = lastItemBackup;
}

void PropertiesWidget::setFilters(const QStringList& filters)
{
    QStandardItemModel *model = mModel;
    QModelIndex index;
    QModelIndexList _indexes;
    QModelIndexList indexes;

    //reset hidden rows
    for (int i=0; i < model->rowCount(); i++) {
        index = model->index(i, 0);
        if (this->isRowHidden(index.row(), index.parent()))
            this->setRowHidden(index.row(), index.parent(), false);
    }

    foreach(const QString& filter, filters) {
        //can't use findItems because we need to search text in UserRole+1 not in DisplayRole
        _indexes = model->match(model->index(0, 0), Qt::UserRole+1, filter, -1, Qt::MatchFixedString | Qt::MatchRecursive);
        for (int i=0; i < _indexes.size(); i++)
            if (! indexes.contains(_indexes[i]))
                indexes.append(_indexes[i]);
    }

    for(int i=0; i < indexes.size(); i++)
        this->setRowHidden(indexes[i].row(), indexes[i].parent(), true);
}

void PropertiesWidget::clear(int from)
{
    this->model()->removeRows(from, this->model()->rowCount());
}

void PropertiesWidget::removeItem(QStandardItem* item)
{
    if (! item)
        return;
    QStandardItem* parent = item->parent();
    if (parent)
        parent->removeRow(item->row());
    else
        this->model()->removeRow(item->row());

    if (parent && ! parent->hasChildren())
        removeItem(parent);
}

PropertiesWidget* PropertiesWidget::copy()
{
    PropertiesWidget* newWidget = new PropertiesWidget();
    newWidget->append(this);
    return newWidget;
}
