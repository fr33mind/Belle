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

#ifndef GOTOSCENE_H
#define GOTOSCENE_H

#include "action.h"

class GoToScene : public Action
{
    Q_OBJECT

public:
    enum TargetType {
        Name=0,
        Position
    };

    static ActionInfo Info;

public:
    explicit GoToScene(QObject *parent = 0);
    GoToScene(const QVariantMap&, QObject *parent);

    void setTargetScene(const QString&, TargetType type=Name);
    QString targetScene();

    virtual QVariantMap toJsonObject();


signals:

public slots:

private:
    void init();

    QString mTargetScene;
    TargetType mTargetType;

};

#endif // GOTOSCENE_H
