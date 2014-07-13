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

#include "show.h"

static ShowEditorWidget* mShowEditorWidget = 0;
ActionInfo Show::Info;

Show::Show(const QVariantMap &data, QObject *parent):
    ChangeVisibility(data, parent)
{
    init();

    if (data.contains("state") && data.value("state").type() == QVariant::String ) {

    }
}

Show::Show(QObject *parent):
    ChangeVisibility(true, parent)
{
    init();
}

void Show::init()
{
    setType(Info.type);
    setName(Info.name);
    setIcon(Info.icon);
    mCharacterState = "";
}

void Show::setCharacterState(const QString& state)
{
    mCharacterState = state;
}

QString Show::characterState()
{
    return mCharacterState;
}

void Show::setShowEditorWidget(ShowEditorWidget * widget)
{
   mShowEditorWidget = widget;
}

ShowEditorWidget* Show::showEditorWidget()
{
   return mShowEditorWidget;
}

ActionEditorWidget* Show::editorWidget()
{
    return mShowEditorWidget;
}

QVariantMap Show::toJsonObject()
{
    QVariantMap data = ChangeVisibility::toJsonObject();

    if (sceneObject() && qobject_cast<Character*>(sceneObject()))
        data.insert("state", mCharacterState);

    return data;
}
