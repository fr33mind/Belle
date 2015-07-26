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

#include "change_game_variable.h"

#include "utils.h"

ActionInfo ChangeGameVariable::Info;

ChangeGameVariable::ChangeGameVariable(QObject *parent) :
    Action(parent)
{
    init();
}

ChangeGameVariable::ChangeGameVariable(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();

    if (data.contains("variable") && data.value("variable").type() == QVariant::String) {
        mVariable = data.value("variable").toString();
    }

    if (data.contains("value") && data.value("value").type() == QVariant::String) {
        mValue = data.value("value").toString();
    }

    if (data.contains("operator") && data.value("operator").type() == QVariant::String) {
        mOperatorIndex = mOperators.indexOf(data.value("operator").toString());
    }
}

void ChangeGameVariable::init()
{
    setType(Info.type);
    setTypeName(Info.typeName);
    setIcon(Info.icon);

    mOperators.append("assign");
    mOperators.append("add");
    mOperators.append("subtract");
    mOperators.append("multiply");
    mOperators.append("divide");
    mOperators.append("append");

    mOperatorIndex = 0;
}

int ChangeGameVariable::operatorIndex()
{
    return mOperatorIndex;
}

void ChangeGameVariable::setOperatorIndex(int index)
{
    if (index >= 0 && index < mOperators.size()) {
        mOperatorIndex = index;
        emit dataChanged();
    }
}

QString ChangeGameVariable::variable()
{
    return mVariable;
}

void ChangeGameVariable::setVariable(const QString & var)
{
    mVariable = var;
    emit dataChanged();
}

QString ChangeGameVariable::value()
{
    return mValue;
}

void ChangeGameVariable::setValue(const QString & val)
{
    mValue = val;
    emit dataChanged();
}

QString ChangeGameVariable::displayText() const
{
    QString variable = tr("Nothing");
    QString value = variable;
    QString op = mOperators[mOperatorIndex];

    if (! mVariable.isEmpty())
        variable = "$" + mVariable;
    if (! mValue.isEmpty()) {
        value = mValue;
        if (! Utils::isNumber(value))
            value = QString("\"%1\"").arg(value);
    }

    return QString("%1 %2 %3 %4").arg(op)
                                 .arg(value)
                                 .arg(tr("to"))
                                 .arg(variable);
}

QVariantMap ChangeGameVariable::toJsonObject()
{
    QVariantMap data = Action::toJsonObject();

    data.insert("variable", mVariable);
    data.insert("operator", mOperators[mOperatorIndex]);
    data.insert("value", mValue);

    return data;
}
