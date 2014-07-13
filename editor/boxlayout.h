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

#ifndef BOXLAYOUT_H
#define BOXLAYOUT_H

#include <QWidget>
#include <QBoxLayout>


class BoxLayout : public QBoxLayout
{
    Q_OBJECT

    QList<QWidget*> mWidgets;

public:
    BoxLayout(Direction dir, QWidget *parent = 0);
    ~BoxLayout();
    void addWidget( QWidget * widget, int stretch = 0, Qt::Alignment alignment = 0 );
    void deleteAllWidgets();
    bool contains(QWidget*);
signals:

public slots:

};

class HBoxLayout : public BoxLayout
{
public:
    explicit HBoxLayout(QWidget *parent = 0);
    ~HBoxLayout();
};

class VBoxLayout : public BoxLayout
{
public:
    explicit VBoxLayout(QWidget *parent = 0);
    ~VBoxLayout();
};


#endif // BOXLAYOUT_H
