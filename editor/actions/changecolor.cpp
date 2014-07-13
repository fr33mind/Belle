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

#include "changecolor.h"

#include "resource_manager.h"
#include "utils.h"

static ChangeColorEditorWidget * mEditorWidget = 0;
ActionInfo ChangeColor::Info;

ChangeColor::ChangeColor(QObject *parent) :
    Action(parent)
{
    init();
    Object * object = qobject_cast<Object*>(parent);
    if (object)
        setSceneObject(object);
}

ChangeColor::ChangeColor(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();

    Object * object = qobject_cast<Object*>(parent);
    if (object)
        setSceneObject(object);

    if (data.contains("color") && data.value("color").type() == QVariant::List)
        mColor = Utils::listToColor(data.value("color").toList());

    if (data.contains("changeObjectColor") && data.value("changeObjectColor").type() == QVariant::Bool)
        mChangeObjectColor = data.value("changeObjectColor").toBool();

    if (data.contains("changeObjectBackgroundColor") && data.value("changeObjectBackgroundColor").type() == QVariant::Bool)
        mChangeObjectBackgroundColor = data.value("changeObjectBackgroundColor").toBool();

}

void ChangeColor::init()
{
    setName(Info.name);
    setType(Info.type);
    setIcon(Info.icon);
    mChangeObjectColor = true;
    mChangeObjectBackgroundColor = false;
    setSupportedEvents(Interaction::MousePress | Interaction::MouseRelease | Interaction::MouseMove);
    mColor = Qt::white;
}

QVariantMap ChangeColor::toJsonObject()
{
    QVariantMap action = Action::toJsonObject();
    action.insert("color", Utils::colorToList(mColor));
    action.insert("changeObjectColor", mChangeObjectColor);
    action.insert("changeObjectBackgroundColor", mChangeObjectBackgroundColor);
    return action;
}

void ChangeColor::setChangeColorEditorWidget(ChangeColorEditorWidget * widget)
{
   mEditorWidget = widget;
}

ChangeColorEditorWidget* ChangeColor::changeColorEditorWidget()
{
   return mEditorWidget;
}

ActionEditorWidget* ChangeColor::editorWidget()
{
    return mEditorWidget;
}

int ChangeColor::opacity() const
{
    return mColor.alpha();
}

void ChangeColor::setOpacity(int a)
{
    mColor.setAlpha(a);
}

QColor ChangeColor::color() const
{
    return mColor;
}

void ChangeColor::setColor(const QColor & color)
{
    mColor = color;
}

QString ChangeColor::displayText() const
{
    QString name(tr("None"));
    if (sceneObject())
        name = sceneObject()->objectName();

    return tr("Change color of '%1' to %2").arg(name).arg(Utils::colorToString(mColor));
}

bool ChangeColor::changeObjectColor()
{
    return mChangeObjectColor;
}


void ChangeColor::setChangeObjectColor(bool change)
{
    mChangeObjectColor = change;
}

bool ChangeColor::changeObjectBackgroundColor()
{
    return mChangeObjectBackgroundColor;
}

void ChangeColor::setChangeObjectBackgroundColor(bool change)
{
    mChangeObjectBackgroundColor = change;
}
