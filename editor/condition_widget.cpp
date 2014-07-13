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

#include "utils.h"

ConditionWidgetDelegate::ConditionWidgetDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{
    mBrushLightRed = QBrush(QColor(255, 0, 0, 100));
    mBrushLightBlue = QBrush(QColor(0, 0, 255, 100));
}

void ConditionWidgetDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant data = index.data(Qt::UserRole+1);
    if (data.type() == QVariant::String && data.toString().contains("LOGICAL_OPERATOR")) {
        if (data.toString().endsWith("AND"))
            painter->fillRect(option.rect, mBrushLightRed);
        else
            painter->fillRect(option.rect, mBrushLightBlue);
    }

    QStyledItemDelegate::paint(painter, option, index);
}

QSize ConditionWidgetDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

ConditionWidget::ConditionWidget(const QString& condition, QWidget *parent) :
    QListView(parent)
{
    QStandardItemModel *model = new QStandardItemModel(this);
    setModel(model);
    setItemDelegate(new ConditionWidgetDelegate(this));
    mOperators << "==" << "!=" << "in" << ">" << ">=" << "<" << "<=";
    mOperatorsText << tr("equal to") << tr("not equal to") << tr("in") << tr("greater than")
               << tr("greater than or equal") << tr("lesser than") << tr("lesser than or equal")
               << tr("true") << tr("false") << tr("defined") << tr("undefined");
    mPrimitives << tr("true") << tr("false") << tr("defined") << tr("undefined");

    setCondition(condition);
}

void ConditionWidget::setCondition(const QString & condition)
{
    //clear all rows
    QStandardItemModel* standardModel = static_cast<QStandardItemModel*>(model());
    standardModel->removeRows(0, standardModel->rowCount());

    bool ok;
    QString symbol("");
    QString name("");
    QString in("in");
    QString _and("and");
    QString _or("or");
    QStringList conditionParts;
    bool string = false;
    QChar c;

    for(int i=0; i < condition.size(); i++) {
        c = condition[i];

        if (c == '"' || c == '\'')
            string = !string;

        //if string add everything to the same value
        if (string) {
            name += c;
            continue;
        }

        if (c.isLetter() || c.isNumber() || c == '"' || c == '\'') {
            if (name.isEmpty()) {
                if (condition.mid(i, in.size()) == in) {
                    conditionParts.append(in);
                    i += in.size();
                }
                else if (condition.mid(i, _and.size()) == _and) {
                    i += _and.size()-1;
                    appendLogicalOperator(tr("AND"), 0);
                }
                else if (condition.mid(i, _or.size()) == _or) {
                    i += _or.size();
                    appendLogicalOperator(tr("OR"), 1);
                }
                else
                    name += c;
            }
            else
                name += c;
        }
        else if (c == '.') {
            if (! string) { //if not inside of a string, check number validity
                if (! name.isEmpty() ){
                    name.toInt(&ok);
                    if (! ok)
                        break;
                }
                else
                    name += "0";
            }
            name += c;
        }
        else {
            if (! name.isEmpty()) {
                conditionParts.append(name);
                name = "";
            }

            if (c == ' ') {
                //do nothing :)
            }
            else if (c == '&' || c == '|') {
                ++i; //jump next character

                if (c == '&')
                    appendLogicalOperator(tr("AND"), 0);
                else
                    appendLogicalOperator(tr("OR"), 1);
            }
            else if (c == '>' || c == '=' || c == '<' || c == '!') {
                symbol += c;
                //in case of ">=" or "<="
                if (i+1 < condition.size() && condition[i+1] == '=') {
                    symbol += condition[i+1];
                    ++i;
                }
            }
            else
                continue;

            if (mOperators.contains(symbol)) {
                conditionParts.append(symbol);
                symbol = "";

            }
        }

        if (conditionParts.size() == 3) {

            if (! mOperators.contains(conditionParts[1]))
                break;

            if (conditionParts[0].size() && conditionParts[0][0].isLetter())
                conditionParts[0].insert(0, "$");
            if (conditionParts[2].size() && conditionParts[2][0].isLetter())
                conditionParts[2].insert(0, "$");

            appendCondition(conditionParts[0], mOperators.indexOf(conditionParts[1]), conditionParts[2]);
            conditionParts.clear();
        }
    }

    if (! name.isEmpty() && conditionParts.size() == 2) {
        conditionParts.append(name);
        if (conditionParts[0].size() && conditionParts[0][0].isLetter())
            conditionParts[0].insert(0, "$");
        if (conditionParts[2].size() && conditionParts[2][0].isLetter())
            conditionParts[2].insert(0, "$");

        appendCondition(conditionParts[0], mOperators.indexOf(conditionParts[1]), conditionParts[2]);
    }
}

QString ConditionWidget::condition()
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->model());
    if (! model)
        return QString("");

    QString data;
    QStringList parts;
    QString condition("");
    int index;

    for(int i=0; i < model->rowCount(); i++) {
        QStandardItem *item = model->item(i);
        if (item->data().type() != QVariant::String)
            continue;
        data = item->data().toString();

        if (data.contains("LOGICAL_OPERATOR:")) {
            if (data.endsWith("AND")) {
                condition += " and ";
                //condition += " && ";
            }
            else
                condition += " or ";
                //condition += " || ";
        }
        else {
            parts = data.split(" ");

            if (parts.size() < 3)
                continue;

            index = parts[1].toInt();

            if (index >= 0 && index < mOperators.size()) {
                condition += QString("%1 %2 %3 ").arg(parts[0]).arg(mOperators[index]).arg(parts[2]);
            }
        }
    }

    return condition;
}

void ConditionWidget::appendCondition(const QString& var, int op, const QString& v)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->model());
    if (! model)
        return;

    QString text("");
    QString value(v);
    QString variable(var);
    QString data("");

    if (value.isEmpty() || (! mPrimitives.contains(value) && ! Utils::isNumber(value) && ! value.contains('"')))
        value = '"' + value + '"';

    if (variable.size() && variable[0].isLetter())
        variable.insert(0, "$");

    if (op == 2) { //"in" operator - value is the variable and vice-versa
           value.remove('"');
           variable.remove('$');
           if (! variable.contains('"'))
               variable = '"' + variable + '"';
    }

    data = QString("%1 %2 %3").arg(variable).arg(op).arg(value);
    if (mPrimitives.contains(value))
        text = QString("%1 is %3").arg(variable).arg(value);
    else
        text = QString("%1 is %2 %3").arg(variable).arg(mOperatorsText[op].toLower()).arg(value);

    QStandardItem* item = new QStandardItem(text);
    item->setData(data);
    model->appendRow(item);
}

void ConditionWidget::appendLogicalOperator(const QString& text, int index)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(this->model());
    if (! model)
        return;

    QStandardItem *item = 0;

    if (model->rowCount() > 0) {
        item = new QStandardItem(text);
        if (index == 0)
            item->setData("LOGICAL_OPERATOR:AND");
        else
            item->setData("LOGICAL_OPERATOR:OR");
        model->appendRow(item);
    }
}

QStringList ConditionWidget::operatorsText()
{
    return mOperatorsText;
}
