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

#ifndef BRANCH_H
#define BRANCH_H

#include "action.h"
#include "complexcondition.h"
#include "gameobjectmanager.h"

class Action;
class BranchEditorWidget;
class GameObjectManager;

class Branch : public Action
{
    Q_OBJECT

public:
    explicit Branch(QObject *parent = 0);
    virtual ~Branch();
    Branch(const QVariantMap& data, QObject *parent);
    AbstractCondition* condition() const;
    virtual QVariantMap toJsonObject(bool internal=true) const;
    QList<Action*> actions(bool) const;
    GameObjectManager* actionManager(bool) const;
    Action* action(int, bool) const;

signals:
    
public slots:
    void setCondition(AbstractCondition*);
    void setCondition(const QString&);
    void updateDisplayText();
    void appendAction(Action*, bool);
    void removeAction(int, bool, bool del = false);
    void removeAction(Action*, bool, bool del=false);
    void onConditionChanged();

private slots:
    void onActionManagerDestroyed(QObject*);

protected:
    virtual void loadData(const QVariantMap&, bool internal=false);
    virtual void connectToResource();

private:
    AbstractCondition* mCondition;
    GameObjectManager* mTrueActions;
    GameObjectManager* mFalseActions;
    void init();
    void connectActions(Branch*, bool);
    GameObjectManager* createActionManager();
};

#endif // BRANCH_H
