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
#include <QMenu>

#include "scene_manager.h"

ResourcesView::ResourcesView(QWidget *parent) :
    PropertiesWidget(parent, 1)
{
    this->setHeaderHidden(true);

    //edit action
    mEditResourceAction = new QAction(QIcon(":/media/object-edit.png"), tr("Edit"), this);
    connect(mEditResourceAction, SIGNAL(triggered()), this, SLOT(onEditResource()));

    //rename action
    mRenameAction = new QAction(QIcon(":/media/edit-clear.png"), tr("Rename"), this);
    connect(mRenameAction, SIGNAL(triggered()), this, SLOT(onRenameActionTriggered()));

    //remove action
    mRemoveAction = new QAction(QIcon(":/media/delete.png"), tr("Remove"), this);
    connect(mRemoveAction, SIGNAL(triggered()), this, SLOT(onRemoveResource()));

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(contextMenuRequested(const QPoint&)));
    //connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onItemDoubleClicked(const QModelIndex&)));
    connect(ResourceManager::instance(), SIGNAL(resourceAdded(GameObject*)), this, SLOT(addObject(GameObject*)));
    connect(ResourceManager::instance(), SIGNAL(resourceRemoved(GameObject*)), this, SLOT(onResourceRemoved(GameObject*)));
    this->setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);
}

void ResourcesView::contextMenuRequested(const QPoint & point)
{
    QMenu menu;
    QModelIndex index;

    if (! selectedIndexes().isEmpty()) {
        index = selectedIndexes().first();
        menu.addAction(mEditResourceAction);
        menu.addAction(mRenameAction);
        menu.addSeparator();
        GameObject* object = this->object(index);
        if (object && object->clones().isEmpty())
            menu.addAction(mRemoveAction);
    }

    menu.exec(mapToGlobal(point));
}

void ResourcesView::addObject(GameObject * object)
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
    connect(object, SIGNAL(nameChanged(const QString&)), this, SLOT(onObjectNameChanged(const QString&)));
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

void ResourcesView::removeItem(GameObject* object, bool del)
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

void ResourcesView::removeObject(GameObject * object, bool del)
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
    GameObject* obj = mItemToObject.value(item);
    scene->addCopyOfObject(obj);
}*/

void ResourcesView::onRenameActionTriggered()
{
    QModelIndexList indexes = selectedIndexes();

    if (! indexes.isEmpty())
        this->edit(indexes.first());
}

void ResourcesView::onResourceRemoved(GameObject* resource)
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
    GameObject* object = this->object(currentIndex());

    if (object)
        emit editResource(object);
}

GameObject* ResourcesView::object(const QModelIndex & index)
{
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(this->model());

    if (! model || ! index.isValid())
        return 0;

    return mItemToObject.value(model->itemFromIndex(index), 0);
}

QStandardItem* ResourcesView::itemFromObject(GameObject* object)
{
    if (! object)
        return 0;

    QHashIterator<QStandardItem*, GameObject*> it(mItemToObject);
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
    GameObject* obj = object(topLeft);
    QString name = topLeft.data().toString();

    if (obj && obj->objectName() != name) {
        if (ResourceManager::instance()->isValidName(name)) {
            obj->setName(name);
            this->model()->setData(topLeft, obj->name(), Qt::DisplayRole);
            if (topLeft == bottomRight)
                this->model()->setData(bottomRight, obj->name(), Qt::DisplayRole);
        }
    }

    PropertiesWidget::dataChanged(topLeft, bottomRight);
}

void ResourcesView::onObjectNameChanged(const QString & name)
{
    GameObject *obj = qobject_cast<GameObject*>(sender());
    if (obj) {
        QStandardItem* item = itemFromObject(obj);
        if (item)
            item->setData(obj->name(), Qt::DisplayRole);
    }
}
