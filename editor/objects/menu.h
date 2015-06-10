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

#ifndef MENU_H
#define MENU_H

#include "objectgroup.h"
#include "menuoption.h"

class MenuOption;

class Menu : public ObjectGroup
{
    Q_OBJECT

    int size;
    Object* mResourceButton;

public:
    explicit Menu(QObject *parent = 0);
    Menu(const QVariantMap&, QObject* parent=0);

    QList<MenuOption*> options() const;
    MenuOption* optionAt(int) const;
    void removeOptionAt(int);
    void setNumberOfOptions(int);
    void addOption(const QString&, const QList<Action*>& actions=QList<Action*>(), const QString& condition="");
    
signals:
    
public slots:

private:
    void init();
    
};

#endif // MENU_H
