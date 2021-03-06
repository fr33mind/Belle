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

#ifndef ACTIONS_MODEL_H
#define ACTIONS_MODEL_H

#include <QStandardItemModel>
#include "action.h"
#include "gameobjectmanager.h"

class GameObjectManager;

class ActionsModel : public QStandardItemModel
{
    Q_OBJECT

    Action* mCurrentAction;
    GameObjectManager* mActionManager;

public:
    explicit ActionsModel(QObject *parent = 0);
    Action* actionForIndex(const QModelIndex&) const;
    QModelIndex indexForAction(Action*) const;
    Action* actionForItem(QStandardItem*) const;
    void setActions(const QList<Action*>&);
    virtual bool dropMimeData ( const QMimeData *, Qt::DropAction, int, int, const QModelIndex &);
    virtual QMimeData* mimeData( const QModelIndexList & ) const;
    virtual QStringList mimeTypes() const;
    void clear();
    void setCurrentAction(Action*);
    Action* currentAction() const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    GameObjectManager* actionManager() const;
    void setActionManager(GameObjectManager*);

protected:
    void appendActionRow(Action*);
    void insertActionRow(int, Action*);
    void loadActionRows();

signals:
    void actionRemoved(int);
    void actionInserted(int, Action*);

public slots:
    void appendAction(Action*);
    void removeAction(int, bool del=false);
    void removeAction(Action*, bool del=false);
    void insertAction(int, Action*);
    void setCurrentIndex(const QModelIndex&);

private slots:
    void updateView();
    void onCurrentActionDestroyed();
    void onItemChanged(QStandardItem*);
    void onActionManagerDestroyed();

private:
    void updateItemAt(int);

};

#endif // ACTIONS_MODEL_H
