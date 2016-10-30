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

Show::Show(const QVariantMap &data, QObject *parent):
    ChangeVisibility(data, parent)
{
    init();
    loadInternal(data);
}

Show::Show(QObject *parent):
    ChangeVisibility(true, parent)
{
    init();
}

void Show::init()
{
    setType(GameObjectMetaType::Show);
    setToShow(true);
    mCharacterState = "";
}

void Show::loadData(const QVariantMap & data, bool internal)
{
    if (!internal)
        ChangeVisibility::loadData(data, internal);

    if (data.contains("state") && data.value("state").type() == QVariant::String) {
        setCharacterState(data.value("state").toString());
    }
}

void Show::setCharacterState(const QString& state)
{
    if (mCharacterState != state) {
        mCharacterState = state;
        emit dataChanged();
    }
}

QString Show::characterState() const
{
    return mCharacterState;
}

QVariantMap Show::toJsonObject(bool internal) const
{
    QVariantMap data = ChangeVisibility::toJsonObject(internal);

    if (sceneObject() && qobject_cast<Character*>(sceneObject()))
        data.insert("state", mCharacterState);

    return data;
}
