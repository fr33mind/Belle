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
    setObjectsSynced(true);
    setWidth(300);
    setSpacing(10);
    addOption("Button 1");
    addOption("Button 2");
}

Menu::Menu(const QVariantMap& data, QObject *parent) :
    ObjectGroup(data, parent)
{
    init();
    loadInternal(data);
    //_fixButtons();
}


void Menu::init()
{
    setType(GameObjectMetaType::Menu);
}

void Menu::loadData(const QVariantMap& data, bool internal)
{
    if (!internal)
        ObjectGroup::loadData(data, internal);
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

void Menu::filterLoadData(QVariantMap & data)
{
    ObjectGroup::filterLoadData(data);
    data.remove("objects");
    data.remove("height");
    data.remove("width");
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
    bool loaded = false;
    int x=0, y=0;
    MenuOption* option = new MenuOption(text, actions, condition, this);
    Menu* menuResource = qobject_cast<Menu*>(resource());
    option->setWidth(this->width());

    if (menuResource) {
        Object* obj = menuResource->object(count());
        if (obj) {
            option->load(obj->toJsonObject());
            x = obj->x() - menuResource->x();
            y = obj->y() - menuResource->y();
            loaded = true;
        }
    }

    if (!loaded) {
        QList<MenuOption*> options = this->options();
        if (!options.isEmpty()) {
            MenuOption* lastOption = options.last();
            option->load(lastOption->toJsonObject());
        }
    }

    if (loaded)
        add(option, x, y);
    else
        append(option, x);
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

QVariantMap Menu::toJsonObject(bool internal) const
{
    QVariantMap data = ObjectGroup::toJsonObject(internal);
    return data;
}
