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

#include "resources_view.h"

#include <QDebug>
#include <QAction>
#include <QStandardItemModel>
#include <QModelIndex>

#include "scene_manager.h"

ResourcesView::ResourcesView(QWidget *parent) :
    PropertiesWidget(parent, 1)
{
    this->setHeaderHidden(true);

    //edit action
    mEditResourceAction = new QAction(QIcon(":/media/object-edit.png"), tr("Edit"), this);
    connect(mEditResourceAction, SIGNAL(triggered()), this, SLOT(onEditResource()));
    addAction(mEditResourceAction);

    //rename action
    QAction* renameAction = new QAction(QIcon(":/media/edit-clear.png"), tr("Rename"), this);
    connect(renameAction, SIGNAL(triggered()), this, SLOT(onRenameActionTriggered()));
    addAction(renameAction);

    //remove action
    QAction* removeAction = new QAction(QIcon(":/media/delete.png"), tr("Remove"), this);
    connect(removeAction, SIGNAL(triggered()), this, SLOT(onRemoveResource()));
    addAction(removeAction);

    setContextMenuPolicy(Qt::ActionsContextMenu);
    //connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onItemDoubleClicked(const QModelIndex&)));
    connect(ResourceManager::instance(), SIGNAL(resourceAdded(Object*)), this, SLOT(addObject(Object*)));
    connect(ResourceManager::instance(), SIGNAL(resourceRemoved(Object*)), this, SLOT(onResourceRemoved(Object*)));
    this->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);
}

void ResourcesView::addObject(Object * object)
{
    if (! object)
        return;

    if (containsGroup(object->type())) {
        setLastItem(object->type());
        appendRow(QIcon(), object->objectName());
    }
    else {
        beginGroup(object->type());
        appendRow(QIcon(), object->objectName());
        endGroup();
    }

    QStandardItem* item = lastItem();
    int rowCount = item->rowCount() ? item->rowCount()-1 : 0;
    item = item->child(rowCount);
    item->setEditable(true);
    mItemToObject.insert(item, object);
    connect(object, SIGNAL(dataChanged(const QVariantMap&)), this, SLOT(onObjectDataChanged(const QVariantMap&)));
}

void ResourcesView::select(const QString& name)
{
    QStandardItem* item = 0;
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(this->model());

    if (! model)
        return;

    for (int i=0; i < model->rowCount(); i++ ) {
        item = model->item(i, 0);
        for (int j=0; j < item->rowCount(); j++)
            if (item->child(j, 0)->text() == name) {
                selectionModel()->select(item->child(j, 0)->index(), QItemSelectionModel::ClearAndSelect);
                return;
            }
    }
}

void ResourcesView::removeItem(Object * object, bool del)
{
    if (! object)
        return;

    QStandardItem *item = 0, *parentItem = 0;
    item = itemFromObject(object);

    if (item) {
        mItemToObject.remove(item);
        //get parent (group) item
        parentItem = item->parent();
        if (parentItem)
            parentItem->removeRow(item->row());
        //check if the group has any more items, if not remove it
        if (parentItem->rowCount() == 0) {
            this->model()->removeRow(parentItem->row());
        }
    }
}

void ResourcesView::removeObject(Object * object, bool del)
{
    if (ResourceManager::instance())
        ResourceManager::instance()->removeResource(object, del);
}

/*void ResourcesView::onItemDoubleClicked(const QModelIndex & index)
{
    if (! SceneManagercurrentScene())
        return;

    const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(index.model());
    if (! model)
        return;

    Scene* scene = SceneManagercurrentScene();
    if (! scene)
        return;

    QStandardItem* item = model->itemFromIndex(index);
    Object* obj = mItemToObject.value(item);
    scene->addCopyOfObject(obj);
}*/

void ResourcesView::onRenameActionTriggered()
{
    QModelIndexList indexes = selectedIndexes();

    if (! indexes.isEmpty())
        this->edit(indexes.first());
}

void ResourcesView::onResourceRemoved(Object* resource)
{
    removeItem(resource);
}

void ResourcesView::onRemoveResource()
{
    QModelIndexList indexes = selectedIndexes();
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(this->model());
    QStandardItem* item = 0;
    if (! model)
        return;

    foreach(const QModelIndex& index, indexes) {
        item = model->itemFromIndex(index);
        removeObject(mItemToObject.value(item, 0), true);
    }
}

void ResourcesView::onEditResource()
{
    Object* object = this->object(currentIndex());

    if (object)
        emit editResource(object);
}

Object* ResourcesView::object(const QModelIndex & index)
{
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(this->model());

    if (! model || ! index.isValid())
        return 0;

    return mItemToObject.value(model->itemFromIndex(index), 0);
}

QStandardItem* ResourcesView::itemFromObject(Object* object)
{
    if (! object)
        return 0;

    QHashIterator<QStandardItem*, Object*> it(mItemToObject);
    QStandardItem *item = 0;

    while(it.hasNext()) {
        it.next();
        if (it.value() == object) {
            item = it.key();
            break;
        }
    }

    return item;
}


void ResourcesView::dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
    Object* obj = object(topLeft);

    if (obj && obj->objectName() != topLeft.data().toString()) {
        bool changed = obj->setName(topLeft.data().toString());
        if (! changed) {
            this->model()->setData(topLeft, obj->name(), Qt::DisplayRole);
            if (topLeft == bottomRight)
                this->model()->setData(bottomRight, obj->name(), Qt::DisplayRole);
        }
    }

    PropertiesWidget::dataChanged(topLeft, bottomRight);
}

void ResourcesView::onObjectDataChanged(const QVariantMap & data)
{
    Object *obj = qobject_cast<Object*>(sender());
    if (obj) {
        QStandardItem* item = itemFromObject(obj);
        if (item && data.contains("name"))
            item->setData(data.value("name").toString(), Qt::DisplayRole);
    }
}
