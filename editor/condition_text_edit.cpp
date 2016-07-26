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
#include <QMouseEvent>
#include <QCursor>
#include <QMessageBox>

#include "condition_dialog.h"
#include "literalcondition.h"

ConditionTextEdit::ConditionTextEdit(QWidget *parent) :
    QTextEdit(parent)
{
    setMouseTracking(true);
    viewport()->setCursor(Qt::PointingHandCursor);
    mEditPixmap = QIcon(":/media/object-edit.png").pixmap(20, 20);
}

void ConditionTextEdit::setCondition(AbstractCondition* condition)
{
    mCondition = condition;
    if (mCondition)
        setPlainText(mCondition->toString());
}

AbstractCondition* ConditionTextEdit::condition() const
{
    return mCondition;
}

void ConditionTextEdit::mouseReleaseEvent(QMouseEvent *e)
{
    QTextEdit::mouseReleaseEvent(e);

    if (mCondition->isLiteral()) {
        QMessageBox::StandardButton btn = QMessageBox::warning(this, tr("Editing Unsupported"), tr("This condition is in a format that can't be edited.\n"
                                                                                                   "You'll have to recreate the condition using the condition editor.\n\n"
                                                                                                   "Do you want to recreate it now?"),
                                                               QMessageBox::No | QMessageBox::Yes);
        if (btn == QMessageBox::No)
            return;

        mCondition = new ComplexCondition();
        emit conditionChanged(mCondition);
    }

    ComplexCondition* ccondition = dynamic_cast<ComplexCondition*>(mCondition);
    if (isReadOnly() && ccondition) {
        ConditionDialog dialog(ccondition);
        dialog.exec();
        setPlainText(mCondition->toString());
        emit conditionChanged();
    }
}

void ConditionTextEdit::mouseMoveEvent(QMouseEvent *e)
{
    QTextEdit::mouseMoveEvent(e);
}

void ConditionTextEdit::paintEvent(QPaintEvent *e)
{
    QTextEdit::paintEvent(e);
}

QPoint ConditionTextEdit::editPixmapPos() const
{
    return QPoint(width() - mEditPixmap.width() - 5, 2);
}
