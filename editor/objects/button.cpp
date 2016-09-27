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

#include "button.h"

#include <QDebug>

#include "scene.h"
#include "changecolor.h"

Button::Button(QObject *parent, const QString& name) :
    TextBox(parent, name)
{
    init();

}
Button::Button(const QString&text, QObject *parent, const QString& name) :
    TextBox(parent, name)
{
    init();
}

Button::Button(const QVariantMap& data, QObject *parent) :
    TextBox(data, parent)
{
    initRect(data);
    setType(GameObjectMetaType::Button);
}

void Button::init()
{
    if (text().isNull() || text().isEmpty())
        setText(tr("Button"));

    QColor color(backgroundColor());
    color.setRgb(220, 220, 220);
    setBackgroundColor(color);
    setBackgroundOpacity(255);
    setTextAlignment(Qt::AlignCenter);
    setCornerRadius(10);
    setBorderColor(Qt::black);
    setBorderWidth(1);
    setType(GameObjectMetaType::Button);

    initRect();

    ChangeColor *changeColor = new ChangeColor(this);
    changeColor->setColor(QColor(Qt::white));
    changeColor->setChangeObjectColor(false);
    changeColor->setChangeObjectBackgroundColor(true);
    appendEventAction(Interaction::MouseMove, changeColor);
    setText(tr("Button"));
}

void Button::initRect(const QVariantMap &data)
{
    if (!data.contains("width") && !data.contains("height")) {
        setWidth(Scene::width()/4);
        setHeight(Scene::height()/9);
    }

    if (!data.contains("x") && !data.contains("y")) {
        setX(Scene::width()/2 - width()/2);
        setY(Scene::height()/2 - height()/2);
    }
}

void Button::move(int x, int y)
{
    TextBox::move(x, y);
}

void Button::setText(const QString & text)
{
    TextBox::setText(text);

    /*int width = this->width();
    int fontWidth = QFontMetrics(QFont()).width(text);
    int leftPadding = 0;
    if (width > fontWidth)
        leftPadding = (width - fontWidth) / 2;

    setTextPadding("left", leftPadding - fontWidth/2);*/
}

void Button::paint(QPainter & painter)
{
    TextBox::paint(painter);
}
