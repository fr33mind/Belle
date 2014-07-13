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

#include "boxlayout.h"

#include <QDebug>

HBoxLayout::HBoxLayout(QWidget *parent) :
    BoxLayout(QBoxLayout::LeftToRight, parent)
{
}

HBoxLayout::~HBoxLayout()
{
}

VBoxLayout::VBoxLayout(QWidget *parent) :
    BoxLayout(QBoxLayout::TopToBottom, parent)
{
}

VBoxLayout::~VBoxLayout()
{
}

BoxLayout::BoxLayout(Direction dir, QWidget *parent) :
    QBoxLayout(dir, parent)
{
}

BoxLayout::~BoxLayout()
{
}


void BoxLayout::addWidget(QWidget* widget, int stretch, Qt::Alignment alignment)
{
    mWidgets.append(widget);
    QBoxLayout::addWidget(widget, stretch, alignment);
}


void BoxLayout::deleteAllWidgets()
{
    qDeleteAll(mWidgets);
}

bool BoxLayout::contains(QWidget* widget)
{
    return mWidgets.contains(widget);
}
