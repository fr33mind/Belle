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

#ifndef GROUP_WIDGET_CONTAINER_H
#define GROUP_WIDGET_CONTAINER_H

#include <QTreeView>
#include <QStandardItemModel>

class CurtainWidget : public QTreeView
{    
    Q_OBJECT

    QStandardItemModel *mModel;

public:
    explicit CurtainWidget(QWidget *parent = 0);
    void addWidget(QWidget* widget);
    void addNewGroup(const QString& title, const QIcon& icon=QIcon());
signals:

public slots:

};

#endif // GROUP_WIDGET_CONTAINER_H
