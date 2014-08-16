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

#include <QDebug>

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

    Button* button = new Button(this);
    button->setResource(mResourceButton);
    button->setWidth(this->width());
    this->append(button);

    button = new Button(this);
    button->setResource(mResourceButton);
    button->setWidth(this->width());
    this->append(button, 10);
}


Menu::Menu(const QVariantMap& data, QObject *parent) :
    ObjectGroup(data, parent)
{
    init();
}


void Menu::init()
{
    setType("Menu");
}

QString Menu::optionText(int index)
{
    Object *obj = object(index);
    if (obj) {
        Button* btn = static_cast<Button*>(obj);
        return btn->text();
    }
    return "";
}

void Menu::setOptionText(int index, const QString& text)
{
    Object *obj = object(index);
    if (obj) {
        Button* btn = static_cast<Button*>(obj);
        btn->setText(text);
        emit dataChanged();
    }
}

QList<Action*> Menu::optionActions(int index)
{
    Object *obj = object(index);
    QList<Action*> actions;
    if (obj) {
        Button* btn = static_cast<Button*>(obj);
        return btn->actionsForEvent(Interaction::MouseRelease);
    }
    return actions;
}


void Menu::appendActionToOption(int index, Action* action)
{
    Object* obj = object(index);

    if (obj)
        obj->appendEventAction(Interaction::MouseRelease, action);
}

void Menu::removeActionFromOption(int optIndex, int actionIndex, bool del)
{
    Object* obj = object(optIndex);

    if (obj)
        obj->removeEventActionAt(Interaction::MouseRelease, actionIndex, del);
}

void Menu::setNumberOfOptions(int number)
{
    if (number == objects().size())
        return;

    for(int i=objects().size()-1; i < number; i++)
        appendOption(QString::number(i+1));

    for(int i=objects().size()-1; i >= number; i--)
        removeObjectAt(i);
}

void Menu::appendOption(const QString& text)
{
    Button* button = new Button(text, this);
    button->setWidth(this->width());
    button->setResource(mResourceButton);
    append(button, 10);
    mConditions.append("");
}

QString Menu::condition(int index)
{
    if (index < mConditions.size())
        return mConditions[index];
    return "";
}

void Menu::setCondition(int index, const QString& condition)
{
    if (index < mConditions.size())
        mConditions.insert(index, condition);
    else
        mConditions.insert(mConditions.size(), condition);
}
