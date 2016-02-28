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

#ifndef RESOURCES_VIEW_H
#define RESOURCES_VIEW_H

#include "properties_widget.h"
#include "objects/object.h"

class ResourcesView : public PropertiesWidget
{
    Q_OBJECT

    QList<GameObject*> mObjects;
    QHash<QStandardItem*, GameObject*> mItemToObject;
    QAction* mEditResourceAction;
    QAction* mRenameAction;
    QAction* mRemoveAction;

public:
    explicit ResourcesView(QWidget *parent = 0);
    void select(const QString&);
    GameObject* object(const QModelIndex&);

protected:
    QStandardItem* itemFromObject(GameObject*);

signals:
    void editResource(GameObject*);

public slots:
    void addObject(GameObject*);
    void onRemoveResource();
    void onRenameActionTriggered();
    void onEditResource();
    //void onItemDoubleClicked(const QModelIndex&);
    void onResourceRemoved(GameObject*);
    void onObjectNameChanged(const QString&);

private slots:
    void contextMenuRequested(const QPoint&);

protected slots:
    virtual void dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> & roles = QVector<int> ());

private:
    void removeObject(GameObject*, bool del=false);
    void removeItem(GameObject*, bool del=false);


};

#endif // RESOURCES_VIEW_H
