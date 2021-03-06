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

#ifndef SHOW_H
#define SHOW_H

#include "change_visibility.h"

class ChangeVisibility;

class Show : public ChangeVisibility
{
    Q_OBJECT

    QString mCharacterState;
    QString mStateImagePath;

public:
    explicit Show(QObject *parent = 0);
    explicit Show(const QVariantMap& data, QObject *parent = 0);
    void setCharacterState(const QString&);
    QString characterState() const;
    virtual QVariantMap toJsonObject(bool internal=true) const;

protected:
    virtual void loadData(const QVariantMap&, bool internal=false);

private:
    void init();
};


#endif // SHOW_H
