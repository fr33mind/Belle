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

#include "menu.h"

#include "button.h"
#include "resource_manager.h"

Menu::Menu(QObject *parent) :
    ObjectGroup(parent)
{
    init();
    setWidth(300);
    mResourceButton = new Button(this);
    mResourceButton->setObjectName("menuButton");
    ResourceManager::instance()->addResource(mResourceButton);

    addOption("Button 1");
    addOption("Button 2");
}

Menu::Menu(const QVariantMap& data, QObject *parent) :
    ObjectGroup(data, parent)
{
    init();
    _fixButtons();

    Object* obj = this->object(0);
    if (obj)
        mResourceButton = qobject_cast<Object*>(obj->resource());
}


void Menu::init()
{
    setType("Menu");
    mResourceButton = 0;
}

//fix buttons for old versions
void Menu::_fixButtons()
{
    QList<Object*> objects = this->objects();
    if (objects.isEmpty() || qobject_cast<MenuOption*>(objects.first()))
        return;

    QList<Object*> new_objects;
    QVariantMap data;

    for(int i=0; i < objects.size(); i++) {
        data = objects.at(i)->toJsonObject();
        data.insert("type", "MenuOption");
        if (data.contains("onMouseRelease")) {
            data.insert("actions", data.value("onMouseRelease").toList());
            data.remove("onMouseRelease");
        }
        new_objects.append(ResourceManager::instance()->createObject(data, this));
    }

    removeAllObjects(true);

    for(int i=0; i < new_objects.size(); i++) {
        _append(new_objects.at(i));
    }
}

QList<MenuOption*> Menu::options() const
{
    QList<MenuOption*> options;
    QList<Object*> objects = this->objects();
    MenuOption* option = 0;
    for(int i=0; i < objects.size(); i++) {
        option = qobject_cast<MenuOption*>(objects.at(i));
        if (option)
            options.append(option);
    }

    return options;
}

MenuOption* Menu::optionAt(int index) const
{
    QList<Object*> objects = this->objects();
    if (index >=0 && index < objects.size())
        return qobject_cast<MenuOption*>(objects.at(index));

    return 0;
}

void Menu::removeOptionAt(int index)
{
    this->removeObjectAt(index, true);
}

void Menu::addOption(const QString& text, const QList<Action*>& actions, const QString& condition)
{
    MenuOption* option = new MenuOption(text, actions, condition, this);
    int spacing = objects().size() > 0 ? 10 : 0;
    option->setWidth(this->width());
    option->setResource(mResourceButton);
    append(option, spacing);
}

void Menu::setNumberOfOptions(int number)
{
    if (number == options().size())
        return;

    for(int i=objects().size(); i < number; i++)
        addOption(QString("%1 %2").arg("Button").arg(QString::number(i+1)));

    for(int i=options().size()-1; i >= number; i--)
        removeOptionAt(i);
}
