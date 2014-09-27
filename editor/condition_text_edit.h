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

#ifndef CONDITION_TEXT_EDIT_H
#define CONDITION_TEXT_EDIT_H

#include <QTextEdit>
#include <QPixmap>

class ConditionTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit ConditionTextEdit(QWidget *parent = 0);

protected:
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);

private:
    QPoint editPixmapPos() const;
private:
    QPixmap mEditPixmap;
};

#endif // CONDITION_TEXT_EDIT_H
