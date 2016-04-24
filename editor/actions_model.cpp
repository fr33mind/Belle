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
    mCurrentScene = 0;
}

void ActionsModel::appendAction(Action* action)
{
    insertAction(rowCount(), action);
}

void ActionsModel::insertAction(int row, Action* action)
{   
    if (! action)
        return;

    connect(action, SIGNAL(dataChanged()), this, SLOT(updateView()), Qt::UniqueConnection);
    QStandardItem *item = new QStandardItem;
    item->setEditable(false);
    insertRow(row, item);
    mActions.insert(row, action);
}

void ActionsModel::removeAction(int index)
{
    removeRow(index);
    mActions.removeAt(index);
}

void ActionsModel::clear()
{
    QStandardItemModel::clear();
    mActions.clear();
    mCurrentAction = 0;
}

Action* ActionsModel::actionForIndex(const QModelIndex & index) const
{
    if (index.row() >= 0 && index.row() < mActions.size())
        return mActions.at(index.row());

    return 0;
}

QModelIndex ActionsModel::indexForAction(Action * action) const
{
    int index = mActions.indexOf(action);
    if (index != -1)
        return this->index(index, 0);
    return QModelIndex();
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
        return true;

    int destRow = -1;
    bool middleRow = false;
    if (parent.isValid())
       destRow = parent.row();

    if (destRow < 0) {
       destRow = row;
       middleRow = true;
    }

    if (destRow < 0)
        destRow = this->rowCount()-1;

    QList<QObject*> objects = mimeData->objects();

    foreach(QObject* obj, objects) {
        Action *action = qobject_cast<Action*>(obj);
        int index = mActions.indexOf(action);
        if (index == -1)
            continue;

        mActions.removeAt(index);
        int destIndex = destRow > index && middleRow ? destRow - 1 : destRow;
        mActions.insert(destIndex, action);
        insertRow(destIndex);
    }

    if (mCurrentScene)
        mCurrentScene->setActions(mActions);

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

    QList<Action*> actions = mActions;
    foreach (const QModelIndex &index, indexes) {
        if (index.isValid())
            mimeData->appendObject(actions[index.row()]);
    }

    mimeData->setData("application/x-objectpointers", QByteArray());

    return mimeData;
}

void ActionsModel::setCurrentAction(Action* action)
{
    if (action == mCurrentAction || (action && !mActions.contains(action)))
        return;

    if (mCurrentAction) {
        mCurrentAction->focusOut();
        mCurrentAction->disconnect(this);
    }

    if (action)
        action->focusIn();

    mCurrentAction = action;

    if (mCurrentAction) {
        connect(mCurrentAction, SIGNAL(destroyed()), this, SLOT(onCurrentActionDestroyed()), Qt::UniqueConnection);
        connect(mCurrentAction, SIGNAL(dataChanged()), this, SLOT(updateView()), Qt::UniqueConnection);
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

void ActionsModel::setCurrentScene(Scene * scene)
{
    clear();

    if (mCurrentScene)
        mCurrentScene->disconnect(this);

    mCurrentScene = scene;

    if (mCurrentScene) {
        connect(mCurrentScene, SIGNAL(destroyed()), this, SLOT(onCurrentSceneDestroyed()), Qt::UniqueConnection);
        setActions(mCurrentScene->actions());
    }
}

void ActionsModel::onCurrentSceneDestroyed()
{
    setCurrentScene(0);
}
