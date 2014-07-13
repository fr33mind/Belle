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

#include "extendedlineedit.h"

ExtendedLineEdit::ExtendedLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    mInactiveText = "";
    mInactiveTextInPlace = false;
    connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdited(const QString&)));
    mStyleSheet = "color: rgb(131, 131, 131);";
}

void ExtendedLineEdit::setInactiveText(const QString & text)
{
    mInactiveText = text;
    if (isEmpty()) {
        mInactiveTextInPlace = true;
        setStyleSheet(mStyleSheet);
    }
}

void ExtendedLineEdit::focusInEvent(QFocusEvent * event)
{
    if (mInactiveTextInPlace) {
        clear();
        setStyleSheet("");
    }

    QLineEdit::focusInEvent(event);
}

void ExtendedLineEdit::focusOutEvent(QFocusEvent * event)
{
    if (mInactiveTextInPlace) {
        setText(mInactiveText);
        setStyleSheet(mStyleSheet);
    }

    QLineEdit::focusOutEvent(event);
}

void ExtendedLineEdit::keyPressEvent(QKeyEvent * event)
{

    /*QKey key = event->key();

    if (key != Qt.Key_Backspace && key != Qt.Key_Enter && key != Qt::Key_Escape)
        self.inactiveTextInPlace = false;*/

    QLineEdit::keyPressEvent(event);
}

void ExtendedLineEdit::onTextEdited(const QString& text)
{
    if (text.isEmpty())
        mInactiveTextInPlace = true;
}



bool ExtendedLineEdit::isEmpty()
{
    if (text().isEmpty() || mInactiveTextInPlace)
        return true;

    return false;
}

