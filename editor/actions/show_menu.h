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

#ifndef SHOW_MENU_H
#define SHOW_MENU_H

#include "action.h"

class ShowMenu : public Action
{
    Q_OBJECT

public:
    explicit ShowMenu(QObject *parent = 0);
     ShowMenu(const QVariantMap&, QObject *parent = 0);
     virtual QVariantMap toJsonObject(bool internal=true) const;
     void setSceneObject(Object*);

     void focusIn();
     void focusOut();

signals:

protected:
    virtual void loadData(const QVariantMap&, bool internal=false);

private slots:
    void updateDisplayText();

private:
    void init();

};

#endif // SHOW_MENU_H
