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

    QList<GameObject*> objects = ResourceManager::instance()->objects(GameObjectMetaType::Button);
    if (objects.isEmpty()) {
        Button* btn = new Button(this);
        btn->setName("menuButton");
        ResourceManager::instance()->add(btn);
        setButtonResource(btn);
    }
    else {
        GameObject* obj = objects.at(0);
        setButtonResource(qobject_cast<Button*>(obj));
    }

    setSpacing(10);
    setWidth(300);
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
    mResourceButton = 0;
}

void Menu::loadData(const QVariantMap& data, bool internal)
{
    if (!internal)
        ObjectGroup::loadData(data, internal);

    if (data.contains("buttonResource")) {
        if (data.value("buttonResource").type() == QMetaType::QObjectStar)
            setButtonResource(data.value("buttonResource").value<Button*>());

        if (data.value("buttonResource").type() == QVariant::String) {
            GameObject* obj = ResourceManager::instance()->object(data.value("buttonResource").toString());
            setButtonResource(qobject_cast<Button*>(obj));
        }
    }
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
    data.remove("width");
    data.remove("height");
    data.remove("alignObjects");
    data.remove("_object");
    data.remove("objects");
    data.remove("editingModeData");
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
    option->setWidth(this->width());
    if (mResourceButton)
        option->load(mResourceButton->toJsonObject());
    option->setResource(mResourceButton);
    append(option);
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

void Menu::setButtonResource(Button * btn)
{
    if (mResourceButton == btn)
        return;

    QVariantMap data;
    QList<Object*> objects = this->objects();
    for(int i=0; i < objects.size(); i++) {
        if (objects[i] && objects[i]->resource() != btn) {
            if (btn) {
                data = btn->toJsonObject();
                data.remove("text");
                objects[i]->load(data);
            }
            objects[i]->setResource(btn);
        }
    }

    mResourceButton = btn;
    if (mResourceButton) {
        connect(mResourceButton, SIGNAL(destroyed()), this, SLOT(onButtonResourceDestroyed()), Qt::UniqueConnection);
        connect(mResourceButton, SIGNAL(resized(int,int)), this, SLOT(onButtonResourceResized(int,int)), Qt::UniqueConnection);
    }

    adaptLayout();
    notify("buttonResource", QVariant::fromValue(qobject_cast<QObject*>(mResourceButton)));
}

Button* Menu::buttonResource() const
{
    return mResourceButton;
}

QVariantMap Menu::toJsonObject(bool internal) const
{
    QVariantMap data = ObjectGroup::toJsonObject(internal);
    if (mResourceButton)
        data.insert("buttonResource", mResourceButton->name());
    return data;
}

void Menu::onButtonResourceDestroyed()
{
    mResourceButton = 0;
}

void Menu::onButtonResourceResized(int w, int h)
{
    adaptLayout();
}
