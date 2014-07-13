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

    QList<Object*> mObjects;
    QHash<QStandardItem*, Object*> mItemToObject;
    QAction* mEditResourceAction;

public:
    explicit ResourcesView(QWidget *parent = 0);
    void select(const QString&);
    Object* object(const QModelIndex&);
protected:
    virtual void dataChanged(const QModelIndex &, const QModelIndex &);
    QStandardItem* itemFromObject(Object*);

signals:
    void editResource(Object*);
    
public slots:
    void addObject(Object*);
    void onRemoveResource();
    void onRenameActionTriggered();
    void onEditResource();
    //void onItemDoubleClicked(const QModelIndex&);
    void onResourceRemoved(Object*);
    void onObjectDataChanged(const QVariantMap&);

private:
    void removeObject(Object*, bool del=false);
    void removeItem(Object*, bool del=false);

    
};

#endif // RESOURCES_VIEW_H
