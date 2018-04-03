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

#ifndef CURTAIN_WIDGET_V2_H
#define CURTAIN_WIDGET_V2_H

#include <QWidget>
#include <QtWidgets/QTreeView>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QResizeEvent>

class PropertiesWidget : public QTreeView
{
    Q_OBJECT

    QStandardItemModel *mModel;
    QStandardItem * mLastItem;
    QVariantList mFilters;

public:
    explicit PropertiesWidget(QWidget *parent = 0, int columns=2);
    void beginGroup(const QString&, const QVariant& data=QVariant());
    void appendRow ( QStandardItem *);
    void appendRow(const QString&, const QString&, const QVariant& data=QVariant());
    void appendRow(const QIcon&, const QString&, const QVariant& data=QVariant());
    void appendRow(const QString&, QWidget*, const QVariant& data=QVariant());
    void appendRow(PropertiesWidget*, QStandardItem*, QStandardItem*);
    void endGroup();
    void beginSubGroup(const QString&, QWidget* widget=0);
    void beginSubGroup(const QString&, const QString&);
    void beginSubGroup(PropertiesWidget*, QStandardItem*, QStandardItem*);
    virtual void setData(const QString&, const QString&);
    bool containsGroup(const QString&, const QVariant& data=QVariant());
    void setGroupName(const QString&);
    void moveToGroup(const QString&);
    QStandardItem * lastItem() const;
    void setLastItem(QStandardItem*);
    void setLastItem(const QString&);
    void append(PropertiesWidget*, int startAt=0, const QStringList& filters=QStringList());
    void clear(int from=0);
    void setFilters(const QVariantList&);
    void addFilters(const QVariantList&);
    int removeFilters(const QVariantList&);
    QVariantList filters() const;
    void removeItem(QStandardItem*);
    PropertiesWidget* copy();
    QStandardItem* groupAt(int) const;
    void setCurrentGroup(int);
    QStandardItem* findItemData(const QVariant&, QStandardItem* rootItem=0) const;

protected:
    void drawRow(QPainter *painter, const QStyleOptionViewItem &options, const QModelIndex &index) const;
    //bool edit ( const QModelIndex & index, EditTrigger trigger, QEvent * event );

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

signals:
    void dataChanged(const QString&, const QString&);

private:
    void updateLastItem();

public slots:

};

#endif // CURTAIN_WIDGET_V2_H
