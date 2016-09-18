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

#ifndef CONDITION_WIDGET_H
#define CONDITION_WIDGET_H

#include <QListView>
#include <QStyledItemDelegate>
#include <QStandardItem>
#include <QMouseEvent>

#include "complexcondition.h"

class AbstractCondition;
class ComplexCondition;

class ConditionWidgetDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ConditionWidgetDelegate(QObject* parent = 0);
    virtual void paint (QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QBrush mBrushLightRed;
    QBrush mBrushLightBlue;
};

class ConditionWidget : public QListView
{
    Q_OBJECT

public:
    ConditionWidget(ComplexCondition*, QWidget *parent = 0);
    void appendCondition(SimpleCondition*);
    void appendCondition(ConditionLogicalOperator::Type, SimpleCondition*);
    void setCondition(ComplexCondition*);
    void initFromCondition(ComplexCondition*);
    QStandardItem* itemForCondition(AbstractCondition*);
    void reload();
    
protected:
    void clear();
    virtual void mouseReleaseEvent(QMouseEvent*);

signals:
    void selectedConditionChanged(AbstractCondition*);

public slots:

protected slots:
    virtual void currentChanged(const QModelIndex &, const QModelIndex &);

private slots:
    void onDeleteTriggered();

private:
    QStandardItemModel* mModel;
    ComplexCondition* mCondition;
    QHash<QStandardItem*, AbstractCondition*> mItemToCondition;
    AbstractCondition* mSelectedCondition;
};

#endif // Condition_WIDGET_H
