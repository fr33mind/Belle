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

#include "actions_model.h"

#include "scene_manager.h"
#include "action.h"
#include "mimedata.h"

ActionsModel::ActionsModel(QObject *parent) :
    QStandardItemModel(parent)
{
    mCurrentAction = 0;
    mActionManager = 0;
    connect(this, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(onItemChanged(QStandardItem*)));
}

void ActionsModel::appendAction(Action* action)
{
    insertAction(rowCount(), action);
}

void ActionsModel::insertAction(int row, Action* action)
{   
    if (! action || !mActionManager)
        return;

    insertActionRow(row, action);
    mActionManager->insert(row, action);
    emit actionInserted(row, action);
}

void ActionsModel::appendActionRow(Action * action)
{
    insertActionRow(rowCount(), action);
}

void ActionsModel::insertActionRow(int row, Action * action)
{
    connect(action, SIGNAL(dataChanged()), this, SLOT(updateView()), Qt::UniqueConnection);
    QStandardItem *item = new QStandardItem;
    item->setEditable(action->isTextEditable());
    insertRow(row, item);
}

void ActionsModel::removeAction(int index, bool del)
{
    if (index < 0 || index >= rowCount() || !mActionManager)
        return;

    removeRow(index);
    mActionManager->removeAt(index, del);
    emit actionRemoved(index);
}

void ActionsModel::removeAction(Action* action, bool del)
{
    if (mActionManager)
        removeAction(mActionManager->indexOf(action), del);
}

void ActionsModel::clear()
{
    QStandardItemModel::clear();

    if (mActionManager) {
        foreach(GameObject* action, mActionManager->objects()) {
            if (action)
                action->disconnect(this);
        }

        mActionManager->disconnect(this);
    }

    mActionManager = 0;
    mCurrentAction = 0;
}

Action* ActionsModel::actionForIndex(const QModelIndex & index) const
{
    if (mActionManager) {
        GameObject* obj = mActionManager->objectAt(index.row());
        return qobject_cast<Action*>(obj);
    }

    return 0;
}

QModelIndex ActionsModel::indexForAction(Action * action) const
{
    if (mActionManager) {
        int index = mActionManager->indexOf(action);
        if (index != -1)
            return this->index(index, 0);
    }

    return QModelIndex();
}

Action* ActionsModel::actionForItem(QStandardItem * item) const
{
    if (item)
        return actionForIndex(item->index());

    return 0;
}

void ActionsModel::updateView()
{
    Action* action = qobject_cast<Action*>(sender());
    if (! action)
        return;

    QModelIndex modelIndex = indexForAction(action);

    //updates the view(s) connected
    if (modelIndex.isValid())
        emit dataChanged(modelIndex, modelIndex);
}

void ActionsModel::setActions(const QList<Action *> & actions)
{
    clear();

    for(int i=0; i < actions.size(); i++)
        appendAction(actions[i]);
}

bool ActionsModel::dropMimeData (const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent )
{
    if (action == Qt::IgnoreAction)
       return true;

    const MimeData *mimeData = dynamic_cast<const MimeData*> (data);
    if (! mimeData)
        return false;

    if (!mActionManager)
        return false;

    beginResetModel();

    int destRow = -1;
    if (parent.isValid())
       destRow = parent.row();

    if (destRow < 0) {
       destRow = row;
    }

    if (destRow < 0)
        destRow = this->rowCount()-1;

    QList<QObject*> objects = mimeData->objects();
    int i = 0;

    foreach(QObject* obj, objects) {
        Action *action = qobject_cast<Action*>(obj);
        int index = mActionManager->indexOf(action);
        if (index == -1)
            continue;

        mActionManager->removeAt(index);
        mActionManager->insert(destRow+i, action);
        i++;
    }

    for(int i=0; i < rowCount(); i++) {
        updateItemAt(i);
    }

    endResetModel();

    return true;
}

QStringList ActionsModel::mimeTypes() const
{
    QStringList types;
    types << "application/x-objectpointers";
    return types;
}

QMimeData* ActionsModel::mimeData ( const QModelIndexList & indexes ) const
{
    MimeData * mimeData = new MimeData;
    QModelIndexList sortedIndexes = indexes;
    GameObject* obj = 0;

    if (!mActionManager)
        return mimeData;

    qSort(sortedIndexes);

    foreach (const QModelIndex &index, sortedIndexes) {
        if (index.isValid()) {
            obj = mActionManager->objectAt(index.row());
            if (obj)
                mimeData->appendObject(obj);
        }
    }

    mimeData->setData("application/x-objectpointers", QByteArray());

    return mimeData;
}

void ActionsModel::setCurrentAction(Action* action)
{
    if (!mActionManager || action == mCurrentAction || (action && !mActionManager->contains(action)))
        return;

    if (mCurrentAction) {
        mCurrentAction->focusOut();
    }

    if (action)
        action->focusIn();

    mCurrentAction = action;

    if (mCurrentAction) {
        connect(mCurrentAction, SIGNAL(destroyed()), this, SLOT(onCurrentActionDestroyed()), Qt::UniqueConnection);
    }
}

void ActionsModel::setCurrentIndex(const QModelIndex & index)
{
    Action* action = actionForIndex(index);
    setCurrentAction(action);
}

Action* ActionsModel::currentAction() const
{
    return mCurrentAction;
}

void ActionsModel::onCurrentActionDestroyed()
{
    mCurrentAction = 0;
}

void ActionsModel::updateItemAt(int index)
{
    QStandardItem* item = this->item(index);
    Action* action = 0;

    if (mActionManager) {
        GameObject* obj = mActionManager->objectAt(index);
        action = qobject_cast<Action*>(obj);
    }

    if (item && action) {
        item->setEditable(action->isTextEditable());
    }
}

//As a precaution, make sure items are not editable by default
Qt::ItemFlags ActionsModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QStandardItemModel::flags(index);

    if (index.isValid()) {
        Action* action = actionForIndex(index);
        if (action && action->isTextEditable())
            return flags | Qt::ItemIsEditable;
    }

    return flags & (~Qt::ItemIsEditable);
}

//This method was created because somehow internally the item changed
//to an icorrect state, after a drop event followed by an item selection in the view.
void ActionsModel::onItemChanged(QStandardItem * item)
{
    Action* action = actionForItem(item);
    if (action && item && item->isEditable() != action->isTextEditable())
        item->setEditable(action->isTextEditable());
}

GameObjectManager* ActionsModel::actionManager() const
{
    return mActionManager;
}

void ActionsModel::setActionManager(GameObjectManager * actionManager)
{
    if (mActionManager == actionManager)
        return;

    clear();

    mActionManager = actionManager;

    if (mActionManager)
        connect(mActionManager, SIGNAL(destroyed()), this, SLOT(onActionManagerDestroyed()));

    loadActionRows();
}

void ActionsModel::onActionManagerDestroyed()
{
    mActionManager = 0;
}

void ActionsModel::loadActionRows()
{
    QStandardItemModel::clear();
    mCurrentAction = 0;

    if (! mActionManager)
        return;

    Action* action = 0;

    foreach(GameObject* obj, mActionManager->objects()) {
        action = qobject_cast<Action*>(obj);
        appendActionRow(action);
    }
}
