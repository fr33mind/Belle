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

#include "lineedit.h"

LineEdit::LineEdit(const QString& inactiveText, QWidget *parent) :
    QLineEdit(parent)
{
    mInactiveTextInPlace = true;
    setPlaceholderText(inactiveText);
    connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdited(const QString&)));
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged(const QString&)));
}

LineEdit::LineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    mInactiveTextInPlace = true;
    connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdited(const QString&)));
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged(const QString&)));
}


void LineEdit::keyPressEvent(QKeyEvent * event)
{
   /*int key = event->key();

   if (key != Qt::Key_Backspace && key != Qt::Key_Enter)
        mInactiveTextInPlace = true;*/

   QLineEdit::keyPressEvent(event);
}

void LineEdit::focusInEvent(QFocusEvent * event)
{
    if (mInactiveTextInPlace) {
        mSettingInactiveText = true;
        clear();
        setStyleSheet("");
        mSettingInactiveText = false;
    }

    QLineEdit::focusInEvent(event);
}

void LineEdit::focusOutEvent(QFocusEvent * event)
{
    setPlaceholderText(mInactiveText);

    QLineEdit::focusOutEvent(event);
}

void LineEdit::onTextEdited(const QString & text)
{
    if (text.isEmpty())
        mInactiveTextInPlace = true;
    else
        mInactiveTextInPlace = false;
}

void LineEdit::onTextChanged(const QString & text)
{
    if (text.isEmpty())
        return;

    if (! mSettingInactiveText) {
        mInactiveTextInPlace = false;
        setStyleSheet("");
    }
}


QString LineEdit::placeholderText() const
{
    return mInactiveText;
}

void LineEdit::setPlaceholderText(const QString &text)
{
   mInactiveText = text;
   if (mInactiveTextInPlace) {
       mSettingInactiveText = true;
       setText(mInactiveText);
       setInactiveTextStyleSheet();
       mSettingInactiveText = false;
   }
}

void LineEdit::setInactiveTextStyleSheet()
{
    setStyleSheet("color: rgb(131, 131, 131);");
}

QString LineEdit::text() const
{
    if (mInactiveTextInPlace)
        return "";

    return QLineEdit::text();
}
