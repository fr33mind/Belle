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

class Action;
class BranchEditorWidget;

class Branch : public Action
{
    Q_OBJECT

public:
    explicit Branch(QObject *parent = 0);
    Branch(const QVariantMap& data, QObject *parent);
    QString condition()const;
    void setCondition(const QString&);
    virtual QVariantMap toJsonObject(bool internal=true) const;
    QList<Action*> actions(bool) const;
    Action* action(int, bool) const;
    void appendAction(Action*, bool);
    void removeAction(int, bool, bool del = false);

signals:
    
public slots:

private:
    QString mCondition;
    QList<Action*> mTrueActions;
    QList<Action*> mFalseActions;
    void init();
    void updateDisplayText();
    
};

#endif // BRANCH_H
