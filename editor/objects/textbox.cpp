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

#include "textbox.h"

#include <QDebug>
#include <QTextCodec>

#include "utils.h"

static TextPropertiesWidget* mEditorWidget = 0;

TextBox::TextBox(QObject *parent, const QString& name) :
    Object(parent, name)
{
    mSceneRect.setY(Scene::height()-(Scene::height()/3));
    mSceneRect.setHeight(Scene::height()/3);
    mSceneRect.setWidth(Scene::width());
    init("");
}

TextBox::TextBox(const QString& text, QObject* parent, const QString& name) :
    Object(parent, name)
{
    mSceneRect.setY(Scene::height()-(Scene::height()/3));
    mSceneRect.setHeight(Scene::height()/3);
    mSceneRect.setWidth(Scene::width());
    init(text);
}

TextBox::TextBox(const QVariantMap & data, QObject * parent):
    Object(data, parent)
{
    init("");

    if (data.contains("text") && data.value("text").type() == QVariant::String) {
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");

        if (codec)
            mText = codec->toUnicode(data.value("text").toByteArray());
        else
            mText = data.value("text").toString();
    }

    if (data.contains("textColor") && data.value("textColor").type() == QVariant::List)
        mTextColor = Utils::listToColor(data.value("textColor").toList());

    if (data.contains("textAlignment") && data.value("textAlignment").type() == QVariant::String) {
        QString align = data.value("textAlignment").toString();
        Qt::Alignment alignment;

        if (align.contains("HCenter", Qt::CaseInsensitive))
            alignment |= Qt::AlignHCenter;
        else if (align.contains("Left", Qt::CaseInsensitive))
            alignment |= Qt::AlignLeft;
        else if (align.contains("Right", Qt::CaseInsensitive))
            alignment |= Qt::AlignRight;

        if (align.contains("VCenter", Qt::CaseInsensitive))
            alignment |= Qt::AlignVCenter;
        else if (align.contains("Top", Qt::CaseInsensitive))
            alignment |= Qt::AlignTop;
        else if (align.contains("Bottom", Qt::CaseInsensitive))
            alignment |= Qt::AlignBottom;

        mTextAlignment = alignment;
    }

    if (data.contains("font") && data.value("font").type() == QVariant::String) {
        QString font = data.value("font").toString();
        mFont.setFamily(Utils::fontFamily(font));
        mFont.setPixelSize(Utils::fontSize(font));
    }

    if (data.contains("placeholderText") && data.value("placeholderText").type() == QVariant::String)
        this->setPlaceholderText(data.value("placeholderText").toString());
}

void TextBox::init(const QString& text)
{
    mText = text;
    mTextColor = QColor(Qt::black);
    mTextRect = sceneRect();
    mPlaceholderText = "";
    mTextAlignment = Qt::AlignLeft | Qt::AlignTop;
    setType("TextBox");
    mFont.setFamily(Object::defaultFontFamily());
    mFont.setPixelSize(Object::defaultFontSize());
}

TextBox::~TextBox()
{
}

QRect TextBox::textRect()
{
    return mTextRect;
}

void TextBox::setTextRect(const QRect & rect)
{
    mTextRect = rect;
}

QString TextBox::text()
{
    return mText;
}

void TextBox::setText(const QString & text)
{
    if (text != mText) {
        mPlaceholderText.clear();
        mText = text;
        emit dataChanged();
    }
}


TextPropertiesWidget* TextBox::textEditorWidget()
{
    return mEditorWidget;
}

ObjectEditorWidget* TextBox::editorWidget()
{
    return mEditorWidget;
}

void TextBox::setTextEditorWidget(TextPropertiesWidget* widget)
{
    if (! widget)
        return;

    if (mEditorWidget)
        mEditorWidget->deleteLater();

    mEditorWidget = widget;
}

QColor TextBox::textColor()
{
    return mTextColor;
}

void TextBox::setTextColor(const QColor& color)
{
    mTextColor = color;
    emit dataChanged();
}

void TextBox::move(int x, int y)
{
    mTextRect.moveTo(x, y);
    Object::move(x, y);
    alignText();
}

void TextBox::setX(int x)
{
    Object::setX(x);
}

void TextBox::setY(int y)
{
    Object::setY(y);
}


void TextBox::setWidth(int w, bool percent)
{
    Object::setWidth(w, percent);
    mTextRect.setWidth(width());
    emit dataChanged();
}

void TextBox::setHeight(int h, bool percent)
{
    Object::setHeight(h, percent);
    mTextRect.setHeight(h);
    emit dataChanged();

}

void TextBox::alignText()
{
    /*QRect rect = sceneRect();
    QFontMetrics metrics(mFont);
    int textWidth = metrics.width(currentText());
    int textHeight = metrics.height();

    if (mTextAlignment == Qt::AlignCenter) {
        qDebug() << "center";
        int hw = rect.width() / 2;
        int hh = rect.height() / 2;
        int htw = textWidth / 2;
        int hth = textHeight / 2;

        mTextRect.setLeft(rect.x() + hw - htw);
        mTextRect.setTop(rect.y() + hh - hth);
        mTextRect.setWidth(textWidth);
        mTextRect.setHeight(textHeight);
    }


    qDebug() << mTextRect << textWidth << textHeight;*/

}

QString TextBox::placeholderText() const {
    return mPlaceholderText;
}

void TextBox::setPlaceholderText(const QString& text)
{
    if (text != mPlaceholderText) {
        mPlaceholderText = text;
        emit dataChanged();
    }
}

void TextBox::paint(QPainter & painter)
{
    Object::paint(painter);

    if (! painter.opacity())
        return;

    QRect rect(sceneRect());
    rect.setWidth(contentWidth());
    rect.setHeight(contentHeight());
    QPen pen(mTextColor);
    painter.save();
    painter.setFont(mFont);
    painter.setPen(pen);
    painter.drawText(rect, mTextAlignment | Qt::TextWordWrap, currentText());
    painter.restore();
}

QVariantMap TextBox::toJsonObject(bool internal)
{
    QVariantMap object = Object::toJsonObject(internal);
    QVariantList color;
    color << mTextColor.red() << mTextColor.green() << mTextColor.blue()
             << mTextColor.alpha();

    object.insert("textColor", color);
    object.insert("text", mText);
    object.insert("textAlignment", textAlignmentAsString());
    if (mFont != Object::defaultFont())
        object.insert("font", Utils::font(mFont.pixelSize(), mFont.family()));

    if (internal) {
        if (! placeholderText().isEmpty())
            object.insert("placeholderText", placeholderText());
    }

    filterResourceData(object);

    return object;
}


QString TextBox::currentText()
{
    if (! mPlaceholderText.isEmpty())
        return mPlaceholderText;
    return mText;
}

Qt::Alignment TextBox::textAlignment()
{
    return mTextAlignment;
}

QString TextBox::textAlignmentAsString()
{
    QString textAlignment("");
    if (mTextAlignment.testFlag(Qt::AlignHCenter))
        textAlignment += "|HCenter";
    else if (mTextAlignment.testFlag(Qt::AlignLeft))
        textAlignment += "|Left";
    else if (mTextAlignment.testFlag(Qt::AlignRight))
        textAlignment += "|Right";

    if (mTextAlignment.testFlag(Qt::AlignVCenter))
        textAlignment += "|VCenter";
    else if (mTextAlignment.testFlag(Qt::AlignTop))
        textAlignment += "|Top";
    else if (mTextAlignment.testFlag(Qt::AlignBottom))
        textAlignment += "|Bottom";

    if (textAlignment.startsWith("|"))
        textAlignment = textAlignment.mid(1);

    return textAlignment;
}

void TextBox::setTextAlignment(Qt::Alignment alignment)
{
    mTextAlignment = alignment;
    emit dataChanged();
}

int TextBox::fontSize()
{
    return mFont.pixelSize();
}

void TextBox::setFontSize(int size)
{
    mFont.setPixelSize(size);
    emit dataChanged();
}

QString TextBox::fontFamily()
{
    return mFont.family();
}

void TextBox::setFontFamily(const QString& family)
{
    mFont.setFamily(family);
    emit dataChanged();
}



