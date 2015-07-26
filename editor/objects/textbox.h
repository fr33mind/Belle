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

#ifndef TEXT_OBJECT_H
#define TEXT_OBJECT_H

#include <QObject>

#include "scene.h"
#include "object.h"

class ObjectEditorWidget;

class TextBox : public Object
{
    Q_OBJECT

    QString mText;
    QColor mTextColor;
    QColor mPlaceholderTextColor;
    QRect mTextRect;
    QString mPlaceholderText;
    Qt::Alignment mTextAlignment;
    QFont mFont;

public:
    explicit TextBox(QObject *parent = 0, const QString& name="TextBox");
    TextBox(const QString&, QObject* parent=0, const QString& name="TextBox");
    TextBox(const QVariantMap&, QObject*);
    virtual ~TextBox();
    QString text();
    virtual void setText(const QString&);
    void setTextColor(const QColor&);
    QColor textColor();
    void setDefaultTextColor(const QColor&);
    QColor defaultTextColor() const;
    void activateDefaultTextColor();
    void setPlaceholderTextColor(const QColor&);
    QColor placeholderTextColor() const;
    QColor currentColor() const;
    QRect textRect();
    QString currentText();
    int fontSize();
    void setFontSize(int);

    QString fontFamily();
    void setFontFamily(const QString&);

    QString placeholderText() const;
    void setPlaceholderText(const QString&);

    void setTextRect(const QRect&);
    virtual void move(int, int);
    void init(const QString&);
    virtual void paint(QPainter &);
    virtual QVariantMap toJsonObject(bool internal=true);

    void setX(int);
    void setY(int);
    virtual void setWidth(int, bool percent=false);
    virtual void setHeight(int, bool percent=false);

    Qt::Alignment textAlignment();
    QString textAlignmentAsString();
    void setTextAlignment(Qt::Alignment);

    void alignText();

signals:

private:
    void _load(const QVariantMap &);

public slots:
    virtual void load(const QVariantMap &);

};

#endif // TEXT_OBJECT_H
