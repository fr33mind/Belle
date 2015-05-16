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

#ifndef ACTION_INFO_H
#define ACTION_INFO_H

#include <QObject>
#include <QIcon>

class ActionInfo {
public:
    QString typeName;
    QString type;
    QIcon icon;

    explicit ActionInfo(QObject *parent = 0);
    ActionInfo(const QIcon&, const QString&, const QString&, QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // ACTION_INFO_H
