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

#ifndef COMBOBOX_LISTVIEW_H
#define COMBOBOX_LISTVIEW_H

#include <QListView>
#include <QStyledItemDelegate>

class ComboBoxItemDelegate : public QStyledItemDelegate
{
   Q_OBJECT
public:
    explicit ComboBoxItemDelegate(QObject* parent = 0);
    bool editorEvent (QEvent * event, QAbstractItemModel * model, const QStyleOptionViewItem & option, const QModelIndex & index);
    void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;

};

class ComboBoxListView : public QListView
{
    Q_OBJECT
public:
    explicit ComboBoxListView(QWidget *parent = 0);

signals:

public slots:

};

#endif // COMBOBOX_LISTVIEW_H
