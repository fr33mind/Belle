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

#ifndef SET_GAME_VARIABLE_H
#define SET_GAME_VARIABLE_H

#include "action.h"

class SetGameVariable : public Action
{
    Q_OBJECT

    QString mVariable;
    QString mValue;
    int mOperatorIndex;
    QStringList mOperators;

public:
    enum ValueType {
        Value=0,
        Variable
    };

    enum Operators{
        Assign=0,
        AssignRandomNumber,
        Add,
        Subtract,
        Multiply,
        Divide,
        Append
    };

    explicit SetGameVariable(QObject *parent = 0);
    SetGameVariable(const QVariantMap& data, QObject *parent = 0);

    int operatorIndex();
    void setOperatorIndex(int);

    QString value();
    void setValue(const QString&);

    QString variable();
    void setVariable(const QString&);

    ValueType valueType() const;
    void setValueType(ValueType);

    QString randomNumberMinimum() const;
    void setRandomNumberMinimum(const QString&);

    QString randomNumberMaximum() const;
    void setRandomNumberMaximum(const QString&);

    QString displayText() const;

    virtual QVariantMap toJsonObject(bool internal=true) const;
    
signals:
    
public slots:

protected:
    virtual void loadData(const QVariantMap&, bool internal=false);

private:
    void init();

    ValueType mValueType;
    QString mRandomNumberMinimum;
    QString mRandomNumberMaximum;
};

#endif // SET_GAME_VARIABLE_H
