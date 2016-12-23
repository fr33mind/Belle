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

#ifndef GOTOLABEL_H
#define GOTOLABEL_H

#include "action.h"
#include "label.h"

class GoToLabel : public Action
{
    Q_OBJECT

    Label * mTargetLabel;
    QString mTargetLabelName;

public:
    explicit GoToLabel(const QString& name="", QObject *parent = 0);
    GoToLabel(const QVariantMap&, QObject *parent);
    virtual QVariantMap toJsonObject(bool internal=true) const;
    void setTargetLabel(Label*);
    void setTargetLabel(const QString&);
    QString targetLabelName();
    virtual void initFrom(Action*);
    virtual Action* copy();
    bool isValidLabel(const QString&);
    bool hasValidLabel();
    QList<Label*> availableLabels() const;
    Label* findLabel(const QString&) const;
    
signals:
    
private slots:
    void removeTargetLabel();
    void onLabelNameChanged(const QString&);
    void updateTargetLabel();

protected:
    virtual void loadData(const QVariantMap&, bool internal=false);
    
private:
    void init();
};

#endif // GOTOLABEL_H
