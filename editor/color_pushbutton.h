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

#ifndef COLOR_PUSHBUTTON_H
#define COLOR_PUSHBUTTON_H

#include <QPushButton>
#include <QColor>

class ColorPushButton : public QPushButton
{
    Q_OBJECT

    QColor mColor;

public:
    explicit ColorPushButton(QWidget *parent = 0, const QColor& color=QColor());
    QColor newColor();
    void setColor(const QColor&);
    QColor color();

signals:
    void colorChosen(const QColor&);

private slots:
    void onClick();

};

#endif // COLOR_PUSHBUTTON_H
