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

#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>
#include <QKeyEvent>
#include <QFocusEvent>

class LineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit LineEdit(const QString& inactiveText="", QWidget *parent = 0);
    explicit LineEdit(QWidget *parent = 0);
    QString placeholderText() const;
    void setPlaceholderText(const QString &);
    QString text() const;

    
signals:
    
private slots:
    void onTextEdited(const QString&);
    void onTextChanged(const QString&);

protected:
    virtual void keyPressEvent(QKeyEvent *);
    virtual void focusInEvent(QFocusEvent *);
    virtual void focusOutEvent(QFocusEvent *);

private:
    void setInactiveTextStyleSheet();
    QString mInactiveText;
    bool mInactiveTextInPlace;
    bool mSettingInactiveText;
};

#endif // LINEEDIT_H
