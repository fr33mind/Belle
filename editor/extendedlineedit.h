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

#ifndef EXTENDEDLINEEDIT_H
#define EXTENDEDLINEEDIT_H

#include <QLineEdit>

class ExtendedLineEdit : public QLineEdit
{
    Q_OBJECT

    bool mInactiveTextInPlace;
    QString mStyleSheet;
    QString mInactiveText;

public:
    explicit ExtendedLineEdit(QWidget *parent = 0);
    bool isEmpty();
    void setInactiveText(const QString&);

protected:
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent*);
    void keyPressEvent(QKeyEvent*);
    void onTextEdited(const QString&);
    void inactiveTextStyleSheet();

signals:

public slots:

};

#endif // EXTENDEDLINEEDIT_H
