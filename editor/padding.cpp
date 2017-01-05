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

#include "padding.h"

Padding::Padding()
{
    init();
}

Padding::Padding(const QVariantMap& data)
{
    init();

    if (data.contains("left") && data.value("left").canConvert(QVariant::Int))
        mPadding.insert("left", data.value("left").toInt());

    if (data.contains("top") && data.value("top").canConvert(QVariant::Int))
        mPadding.insert("top", data.value("top").toInt());

    if (data.contains("bottom") && data.value("bottom").canConvert(QVariant::Int))
        mPadding.insert("bottom", data.value("bottom").toInt());

    if (data.contains("right") && data.value("right").canConvert(QVariant::Int))
        mPadding.insert("right", data.value("right").toInt());
}

void Padding::init()
{
    mPadding.insert("left", 0);
    mPadding.insert("top", 0);
    mPadding.insert("bottom", 0);
    mPadding.insert("right", 0);
}

int Padding::padding(const QString & side)
{
    return mPadding.value(side);
}

void Padding::setPadding(const QString & side, int value)
{
    mPadding.insert(side, value);
}

int Padding::left() const
{
    return mPadding.value("left");
}

void Padding::setLeft(int left)
{
   mPadding.insert("left", left);
}

int Padding::top() const
{
   return mPadding.value("top");
}

void Padding::setTop(int top)
{
    mPadding.insert("top", top);
}

int Padding::bottom() const
{
    return mPadding.value("bottom");
}

void Padding::setBottom(int bottom)
{
    mPadding.insert("bottom", bottom);
}

int Padding::right() const
{
    return mPadding.value("right");
}

void Padding::setRight(int right)
{
    mPadding.insert("right", right);
}

bool Padding::isEmpty() const
{
    QHashIterator<QString, int> it(mPadding);
    int total = 0;
    while(it.hasNext()) {
        it.next();
        total += it.value();
    }

    return total ? false : true;
}

QVariantMap Padding::toJsonObject() const
{
    QVariantMap object;

    QHashIterator<QString, int> it(mPadding);
    while(it.hasNext()) {
        it.next();
        object.insert(it.key(), it.value());
    }

    return object;
}
