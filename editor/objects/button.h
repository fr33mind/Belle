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

#ifndef BUTTON_H
#define BUTTON_H

#include "textbox.h"

class Button : public TextBox
{
    Q_OBJECT

public:
    explicit Button(QObject *parent = 0, const QString& name="");
    Button(const QString& text, QObject *parent = 0, const QString& name="");
    explicit Button(const QVariantMap&, QObject *parent = 0);
    virtual void paint(QPainter &);
    virtual void move(int, int);
    void setText(const QString &);

private:
    void init();
    void initRect(const QVariantMap&data=QVariantMap());
};

#endif // BUTTON_H
