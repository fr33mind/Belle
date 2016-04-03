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

#ifndef GET_USER_INPUT_H
#define GET_USER_INPUT_H

#include "action.h"

class GetUserInput : public Action
{
    Q_OBJECT

public:
    explicit GetUserInput(QObject *parent = 0);
    GetUserInput(const QVariantMap&, QObject *parent = 0);

    QString variable();
    void setVariable(const QString&);

    QString message();
    void setMessage(const QString&);

    QString defaultValue();
    void setDefaultValue(const QString&);

    virtual QString displayText() const;

    virtual QVariantMap toJsonObject(bool internal=true) const;
    
signals:
    
public slots:

private:
    void rebuildDisplayText();

private:
    QString mVariable;
    QString mMessage;
    QString mDefaultValue;

    void init();
    
};

#endif // GET_USER_INPUT_H
