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

#include "condition_widget.h"

#include <QStandardItemModel>
#include <QPainter>
#include <QDebug>
#include <QAction>

#include "utils.h"

ConditionWidgetDelegate::ConditionWidgetDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{
}

void ConditionWidgetDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

QSize ConditionWidgetDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

ConditionWidget::ConditionWidget(ComplexCondition* condition, QWidget *parent) :
    QListView(parent)
{
    mModel = new QStandardItemModel(this);
    setModel(mModel);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setItemDelegate(new ConditionWidgetDelegate(this));
    setCondition(condition);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    QAction* deleteAction = new QAction(QIcon(":/media/delete.png"), tr("Delete"), this);
    deleteAction->setShortcut(QKeySequence::Delete);
    deleteAction->setShortcutContext(Qt::WidgetShortcut);
    addAction(deleteAction);
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(onDeleteTriggered()));
}

void ConditionWidget::setCondition(ComplexCondition* condition)
{
    mCondition = condition;
    initFromCondition(condition);
}

void ConditionWidget::initFromCondition(ComplexCondition* condition)
{
    if (!mModel || !condition)
        return;

    clear();

    QList<ConditionToken*> tokens = condition->tokens();
    QStandardItem* item = 0;

    foreach(ConditionToken* token, tokens) {
        if (token->type() == ConditionTokenMetaType::LogicalOperator &&
            token->value().toInt() == (int) ConditionLogicalOperator::And)
            continue;

        item = new QStandardItem(token->toString());
        mModel->appendRow(item);
        if (token->type() == ConditionTokenMetaType::SimpleCondition ||
            token->type() == ConditionTokenMetaType::LiteralCondition ||
            token->type() == ConditionTokenMetaType::ComplexCondition)
            mItemToCondition.insert(item, dynamic_cast<AbstractCondition*>(token));
    }
}

void ConditionWidget::appendCondition(ConditionLogicalOperator::Type op, SimpleCondition* condition)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->model());
    if (! model || !mCondition)
        return;

    QStandardItem* item = 0;
    if (op == ConditionLogicalOperator::Or && model->rowCount() > 0) {
        item = new QStandardItem(ConditionLogicalOperator::toString(ConditionLogicalOperator::Or));
        item->setData(ConditionLogicalOperator::Or);
        model->appendRow(item);
    }

    item = new QStandardItem(condition->toString());
    mItemToCondition.insert(item, condition);
    model->appendRow(item);
    mCondition->addCondition(op, condition);
}

void ConditionWidget::appendCondition(SimpleCondition* condition)
{
    appendCondition(ConditionLogicalOperator::And, condition);
}

void ConditionWidget::onDeleteTriggered()
{
    QModelIndexList indexes = selectedIndexes();
    QStandardItem* item = 0;
    AbstractCondition* condition = 0;
    qSort(indexes);
    for(int i=indexes.size()-1; i >= 0; --i) {
        item = mModel->itemFromIndex(indexes[i]);
        if (item && mItemToCondition.contains(item)) {
            condition = mItemToCondition.take(item);
            mCondition->removeCondition(condition, true);
            mModel->removeRow(item->row());
        }
    }

    initFromCondition(mCondition);
}

void ConditionWidget::clear()
{
    mModel->clear();
    mItemToCondition.clear();
}
