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

class ActionsModel : public QStandardItemModel
{
    Q_OBJECT

    Action* mCurrentAction;

public:
    explicit ActionsModel(QObject *parent = 0);
    void insertAction(int, Action*);
    Action* actionForIndex(const QModelIndex&) const;
    void setActions(const QList<Action*>&);
    virtual bool dropMimeData ( const QMimeData *, Qt::DropAction, int, int, const QModelIndex &);
    virtual QMimeData* mimeData( const QModelIndexList & ) const;
    virtual QStringList mimeTypes() const;

signals:

public slots:
    void appendAction(Action*);
    void removeAction(int);
    void setCurrentAction(const QModelIndex&);
    void onCurrentActionDestroyed();

private slots:
    void updateView();

};

#endif // ACTIONS_MODEL_H
