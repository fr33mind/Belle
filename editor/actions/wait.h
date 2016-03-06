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

#ifndef WAIT_H
#define WAIT_H

#include <QVariant>

#include "action.h"

class Action;

class Wait : public Action
{
    Q_OBJECT

public:
    static ActionInfo Info;

public:

    enum WaitType {
        Timed=0,
        MouseClick,
        Forever
    };

    explicit Wait(QObject *parent = 0);
    Wait(const QVariantMap&, QObject*);
    void setTime(double);
    double time();
    WaitType waitType();
    void setWaitType(WaitType);
    void setWaitTypeFromIndex(int);
    QString waitTypeToString(WaitType) const;
    virtual QVariantMap toJsonObject(bool internal=true) const;

signals:

public slots:

private:
    double mTime;
    WaitType mWaitType;
    void init();

};

#endif // WAIT_H
