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

#include "set_game_variable.h"
#include "utils.h"

SetGameVariable::SetGameVariable(QObject *parent) :
    Action(parent)
{
    init();
}

SetGameVariable::SetGameVariable(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();
    loadInternal(data);
}

void SetGameVariable::init()
{
    setType(GameObjectMetaType::SetGameVariable);

    mOperators.append("assign");
    mOperators.append("assign random number");
    mOperators.append("add");
    mOperators.append("subtract");
    mOperators.append("multiply");
    mOperators.append("divide");
    mOperators.append("append");

    mOperatorIndex = 0;
    mValueType = SetGameVariable::Value;
}

void SetGameVariable::loadData(const QVariantMap & data, bool internal)
{
    if(!internal)
        Action::loadData(data, internal);

    if (data.contains("variable") && data.value("variable").type() == QVariant::String) {
        setVariable(data.value("variable").toString());
    }

    if (data.contains("value") && data.value("value").type() == QVariant::String) {
        setValue(data.value("value").toString());
    }

    if (data.contains("operator") && data.value("operator").type() == QVariant::String) {
        setOperatorIndex(mOperators.indexOf(data.value("operator").toString()));
    }

    if (data.contains("valueType") && data.value("valueType").canConvert(QVariant::Int)) {
        setValueType(static_cast<SetGameVariable::ValueType>(data.value("valueType").toInt()));
    }

    if (data.contains("randNumMin") && data.value("randNumMin").type() == QVariant::String) {
        setRandomNumberMinimum(data.value("randNumMin").toString());
    }

    if (data.contains("randNumMax") && data.value("randNumMax").type() == QVariant::String) {
        setRandomNumberMaximum(data.value("randNumMax").toString());
    }
}

int SetGameVariable::operatorIndex()
{
    return mOperatorIndex;
}

void SetGameVariable::setOperatorIndex(int index)
{
    if (mOperatorIndex == index || index < 0 || index >= mOperators.size())
        return;

    mOperatorIndex = index;
    notify("operator", mOperators[mOperatorIndex]);
}

QString SetGameVariable::variable()
{
    return mVariable;
}

void SetGameVariable::setVariable(const QString & var)
{
    if (mVariable == var)
        return;

    mVariable = var;
    notify("variable", mVariable);
}

QString SetGameVariable::value()
{
    return mValue;
}

void SetGameVariable::setValue(const QString & val)
{
    if (mValue == val)
        return;

    mValue = val;
    notify("value", mValue);
}

SetGameVariable::ValueType SetGameVariable::valueType() const
{
    return mValueType;
}

void SetGameVariable::setValueType(ValueType type)
{
    mValueType = type;
    notify("valueType", static_cast<int>(type));
}

QString SetGameVariable::randomNumberMinimum() const
{
    return mRandomNumberMinimum;
}

void SetGameVariable::setRandomNumberMinimum(const QString& number)
{
    mRandomNumberMinimum = number;
    notify("randNumMin", number);
}

QString SetGameVariable::randomNumberMaximum() const
{
    return mRandomNumberMaximum;
}

void SetGameVariable::setRandomNumberMaximum(const QString& number)
{
    mRandomNumberMaximum = number;
    notify("randNumMax", number);
}

QString SetGameVariable::displayText() const
{
    QString variable = tr("Nothing");
    QString value = variable;
    QString op = mOperators[mOperatorIndex];
    QString text, word;

    op[0] = op[0].toUpper();

    if (! mVariable.isEmpty())
        variable = "$" + mVariable;
    if (! mValue.isEmpty()) {
        if (mValueType == SetGameVariable::Variable) {
            value = "$" + mValue;
        }
        else {
            value = mValue;
            //Always show append operation as string
            if (mOperatorIndex == SetGameVariable::Append || ! Utils::isNumber(value))
                value = QString("\"%1\"").arg(value);
        }
    }

    if (mOperatorIndex == SetGameVariable::AssignRandomNumber) {
        //: This "to" refers to "Assign random number".
        word = tr("to", "assignment");
        text = QString("%1 [%2, %3] %4 %5").arg(op)
                                          .arg(mRandomNumberMinimum)
                                          .arg(mRandomNumberMaximum)
                                          .arg(word)
                                          .arg(variable);
    }
    else if (mOperatorIndex == SetGameVariable::Multiply || mOperatorIndex == SetGameVariable::Divide) {
        //: This "by" refers to multiplication/division, i.e.: multiply 10 by 20.
        word = tr("by");
        text = QString("%1 %2 %3 %4").arg(op)
                                     .arg(variable)
                                     .arg(word)
                                     .arg(value);
    }
    else {
        //: This "to" refers to addition, i.e.: add 10 to 20.
        word = tr("to", "addition");
        if (mOperatorIndex == SetGameVariable::Subtract) {
            //: This "from" refers to subtraction, i.e.: subtract 10 from 20.
            word = tr("from");
        }

        text = QString("%1 %2 %3 %4").arg(op)
                                     .arg(value)
                                     .arg(word)
                                     .arg(variable);
    }

    return text;
}

QVariantMap SetGameVariable::toJsonObject(bool internal) const
{
    QVariantMap data = Action::toJsonObject(internal);

    data.insert("variable", mVariable);
    data.insert("operator", mOperators[mOperatorIndex]);
    data.insert("value", mValue);
    data.insert("valueType", static_cast<int>(mValueType));

    if (mOperatorIndex == SetGameVariable::AssignRandomNumber) {
        data.insert("randNumMin", mRandomNumberMinimum);
        data.insert("randNumMax", mRandomNumberMaximum);
    }

    return data;
}
