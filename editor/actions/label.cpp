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

#include "label.h"

Label::Label(const QString& name, QObject *parent) :
    Action(parent)
{
    setObjectName(name);
    init();
}

Label::Label(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();
    setDisplayText(name());
}

void Label::init()
{
    setType(GameObjectMetaType::Label);
}

void Label::setObjectName(const QString & name)
{
    QObject::setObjectName(name);
    setDisplayText(name);
}
