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

#include "condition_text_edit.h"

#include <QDebug>
#include <QPainter>

#include "condition_dialog.h"

ConditionTextEdit::ConditionTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
    setMouseTracking(true);
    mEditPixmap = QIcon(":/media/object-edit.png").pixmap(20, 20);
}

void ConditionTextEdit::mouseReleaseEvent(QMouseEvent *e)
{
    QTextEdit::mouseReleaseEvent(e);

    QPoint pos = editPixmapPos();
    if (e->x() > pos.x() && e->y() < pos.y() + mEditPixmap.height()) {
        ConditionDialog dialog(this->toPlainText());
        dialog.exec();

        if (dialog.result() == QDialog::Accepted) {
            setPlainText(dialog.condition());
        }
        e->accept();
    }
}

void ConditionTextEdit::mouseMoveEvent(QMouseEvent *e)
{
    QTextEdit::mouseMoveEvent(e);

    if (! viewport())
        return;

    QPoint pos = editPixmapPos();
    if (e->x() > pos.x() && e->y() < pos.y() + mEditPixmap.height()) {
        viewport()->setCursor(Qt::PointingHandCursor);
    }
    else if (viewport()->cursor().shape() != Qt::IBeamCursor) {
        viewport()->setCursor(Qt::IBeamCursor);
    }
}

void ConditionTextEdit::paintEvent(QPaintEvent *e)
{
    QTextEdit::paintEvent(e);
    QPainter p(viewport());
    QPoint pos = editPixmapPos();
    p.drawPixmap(pos.x(), pos.y(), mEditPixmap);
}

QPoint ConditionTextEdit::editPixmapPos() const
{
    return QPoint(width() - mEditPixmap.width() - 5, 2);
}
