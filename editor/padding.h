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

#ifndef PADDING_H
#define PADDING_H

#include <QVariant>

class Padding
{
    int mLeft;
    int mTop;
    int mBottom;
    int mRight;

    QHash<QString, int> mPadding;

public:
    explicit Padding();
    Padding(const QVariantMap& data);

    int padding(const QString&);
    void setPadding(const QString&, int);

    int left() const;
    void setLeft(int);

    int top() const;
    void setTop(int);

    int bottom() const;
    void setBottom(int);

    int right() const;
    void setRight(int);

    bool isEmpty() const;
    QVariantMap toJsonObject() const;

private:
    void init();
};

#endif // PADDING_H
