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

#include "menu.h"
#include "scene.h"
#include "resource_manager.h"

ShowMenu::ShowMenu(QObject *parent) :
    Action(parent)
{
     init();
     setSceneObject(new Menu(this));

     Menu* menuResource = 0;
     QList<GameObject*> menus = ResourceManager::instance()->objects(GameObjectMetaType::Menu);

     if (menus.isEmpty()) {
        menuResource = new Menu(this);
        ResourceManager::instance()->add(menuResource);
     }
     else {
         menuResource = qobject_cast<Menu*>(menus.at(0));
     }

     setMenuResource(menuResource);
     setMenuHAlignment("center");
     setMenuVAlignment("center");
}

ShowMenu::ShowMenu(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();
    loadInternal(data);

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
    setType(GameObjectMetaType::ShowMenu);
    setSupportedEvents(Interaction::MousePress | Interaction::MouseRelease);
    mMenuResource = 0;
}

void ShowMenu::loadData(const QVariantMap & data, bool internal)
{
    if (!internal)
        Action::loadData(data, internal);

    if (data.contains("object") && data.value("object").type() == QVariant::Map) {
        Object* object = ResourceManager::instance()->createObject(data.value("object").toMap(), this);
        setSceneObject(object);
    }

    if (data.contains("menuResource")) {
        if (data.value("menuResource").type() == QMetaType::QObjectStar)
            setMenuResource(data.value("menuResource").value<Menu*>());

        if (data.value("menuResource").type() == QVariant::String) {
            GameObject* obj = ResourceManager::instance()->object(data.value("menuResource").toString());
            setMenuResource(qobject_cast<Menu*>(obj));
        }
    }

    if (data.value("menuHA").type() == QVariant::String) {
        setMenuHAlignment(data.value("menuHA").toString());
    }

    if (data.value("menuVA").type() == QVariant::String) {
        setMenuVAlignment(data.value("menuVA").toString());
    }
}

QVariantMap ShowMenu::toJsonObject(bool internal) const
{
    QVariantMap object = Action::toJsonObject(internal);

    if (sceneObject()) {
        object.insert("object", sceneObject()->toJsonObject(internal));
        object.insert("menuHA", mMenuHAlignment);
        object.insert("menuVA", mMenuVAlignment);
    }

    if (mMenuResource)
        object.insert("menuResource", mMenuResource->name());

    return object;
}

void ShowMenu::updateDisplayText()
{
    QStringList lines;
    QString text;
    Menu* menu = static_cast<Menu*>(this->sceneObject());
    QList<MenuOption*> options = menu->options();
    QList<Action*> actions;
    Action* action = 0;
    QStringList actionsTextList;
    const GameObjectMetaType* metatype = 0;

    for(int i=0; i < 3 && i < options.size(); i++) {
        MenuOption* option = options.at(i);
        if (option) {
            actions = option->actions();
            actionsTextList.clear();
            for(int j=0; j < actions.size(); j++) {
                action = actions.at(j);
                if (action) {
                    metatype = GameObjectMetaType::metaType(action->type());
                    if (metatype)
                        actionsTextList.append(metatype->name());
                }
            }

            QString cond = option->condition();
            if (!cond.isEmpty())
                cond = QString(" (if %1)").arg(cond);
            QString actionsText = actionsTextList.join(", ");
            if (actionsText.isEmpty())
                actionsText = tr("Do nothing");
            text = QString("\"%1\"%2: %3").arg(option->text()).arg(cond).arg(actionsText);
            lines.append(text);
        }
    }

    if (options.size() > 3)
        lines.append("...");

    setDisplayText(lines.join("\n"));
}

void ShowMenu::connectSceneObject()
{
    Action::connectSceneObject();
    Object* obj = sceneObject();
    if (obj) {
        connect(obj, SIGNAL(dataChanged()), this, SLOT(updateDisplayText()), Qt::UniqueConnection);
        updateDisplayText();
    }
}

void ShowMenu::disconnectSceneObject()
{
    Action::disconnectSceneObject();
    Object* obj = sceneObject();
    if (obj && obj->parent() == this)
        delete obj;
}

void ShowMenu::loadSceneObject()
{
    Action::loadSceneObject();

    if (sceneObject() && scene())
        scene()->appendObject(sceneObject(), true, true);
}

void ShowMenu::restoreSceneObject()
{
    Action::restoreSceneObject();

    if (sceneObject() && scene())
        scene()->removeObject(sceneObject(), false);
}

void ShowMenu::setMenuResource(Menu * menu)
{
    if (mMenuResource == menu)
        return;

    if (mMenuResource) {
        mMenuResource->disconnect(this);
    }

    mMenuResource = menu;

    if(mMenuResource) {
        connect(mMenuResource, SIGNAL(destroyed()), this, SLOT(onMenuResourceDestroyed()), Qt::UniqueConnection);

        menu = qobject_cast<Menu*>(sceneObject());
        if (menu && menu->resource() != mMenuResource) {
            QList<MenuOption*> options = menu->options();
            menu->load(mMenuResource->toJsonObject());
            QVariantMap data;
            MenuOption* opt=0;
            MenuOption* option=0;

            for(int i=0; i < options.size(); i++) {
                option = options.at(i);
                if (!option)
                    continue;

                opt = mMenuResource->optionAt(i);
                if (opt)
                    data = opt->toJsonObject();

                if (!data.isEmpty()) {
                    data.remove("text");
                    data.remove("actions");
                    option->load(data);
                    if (opt) {
                        option->setX(menu->x() + (opt->x() - mMenuResource->x()));
                        option->setY(menu->y() + (opt->y() - mMenuResource->y()));
                    }
                }
            }

            menu->setResource(mMenuResource);
            menu->update();
            menu->adaptSize();
        }
    }

    notify("menuResource", QVariant::fromValue(qobject_cast<QObject*>(mMenuResource)));
}

void ShowMenu::onMenuResourceDestroyed()
{
    mMenuResource = 0;
}

QString ShowMenu::menuHAlignment() const
{
    return mMenuHAlignment;
}

void ShowMenu::setMenuHAlignment(const QString& alignment)
{
    mMenuHAlignment = alignment.toLower();

    Object* obj = sceneObject();
    if (obj) {
        obj->alignHorizontally(mMenuHAlignment);
    }

    notify("menuHA", mMenuHAlignment);
}

QString ShowMenu::menuVAlignment() const
{
    return mMenuVAlignment;
}

void ShowMenu::setMenuVAlignment(const QString& alignment)
{
    mMenuVAlignment = alignment.toLower();

    Object* obj = sceneObject();
    if (obj) {
        obj->alignVertically(mMenuVAlignment);
    }

    notify("menuVA", mMenuVAlignment);
}
