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

#include "show_menu.h"

#include <QDebug>

#include "menu.h"
#include "scene_manager.h"
#include "resource_manager.h"

static ShowMenuEditorWidget* mEditorWidget = 0;
ActionInfo ShowMenu::Info;

ShowMenu::ShowMenu(QObject *parent) :
    Action(parent)
{
     init();
     setSceneObject(new Menu(this));
}

ShowMenu::ShowMenu(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();

    if (data.contains("object") && data.value("object").type() == QVariant::Map) {
        Object* object = ResourceManager::instance()->createObject(data.value("object").toMap(), this);
        setSceneObject(object);
    }

    //in case of an invalid Menu object
    if (sceneObject()) {
        Menu * menu = qobject_cast<Menu*>(sceneObject());
        if (! menu) {
            sceneObject()->deleteLater();
            setSceneObject(0);
        }
    }

    if (! sceneObject()) {
        setSceneObject(new Menu(this));
    }

}

void ShowMenu::init()
{
    setType(Info.type);
    setName(Info.name);
    setIcon(Info.icon);
    setSupportedEvents(Interaction::MousePress | Interaction::MouseRelease);
}

void ShowMenu::setShowMenuEditorWidget(ShowMenuEditorWidget * widget)
{
   mEditorWidget = widget;
}

ShowMenuEditorWidget* ShowMenu::showMenuEditorWidget()
{
   return mEditorWidget;
}

ActionEditorWidget* ShowMenu::editorWidget()
{
    return mEditorWidget;
}

QVariantMap ShowMenu::toJsonObject()
{
    QVariantMap object = Action::toJsonObject();

    if (sceneObject())
        object.insert("object", sceneObject()->toJsonObject());

    return object;
}

void ShowMenu::focusIn()
{
    if (sceneObject() && this->scene())
        this->scene()->appendObject(sceneObject(), true, true);
}

void ShowMenu::focusOut()
{
    if (sceneObject() && this->scene()) {
        this->scene()->removeObject(sceneObject(), false);
    }
}

