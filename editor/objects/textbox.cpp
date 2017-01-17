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

#include <QTextCodec>

#include "utils.h"
#include "fontlibrary.h"

TextBox::TextBox(QObject *parent, const QString& name) :
    Object(parent, name)
{
    init("");
}

TextBox::TextBox(const QString& text, QObject* parent, const QString& name) :
    Object(parent, name)
{
    init(text);
}

TextBox::TextBox(const QVariantMap & data, QObject * parent):
    Object(data, parent)
{
    init("", data);
    loadInternal(data);
}

void TextBox::loadData(const QVariantMap& data, bool internal)
{
    if (!internal)
        Object::loadData(data, internal);

    if (data.contains("text") && data.value("text").type() == QVariant::String) {
        QTextCodec *codec = QTextCodec::codecForName("UTF-8");

        if (codec)
            mText = codec->toUnicode(data.value("text").toByteArray());
        else
            mText = data.value("text").toString();
    }

    if (data.contains("textColor") && data.value("textColor").type() == QVariant::List)
        setTextColor(Utils::listToColor(data.value("textColor").toList()));

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

        setTextAlignment(alignment);
    }

    //Font string is deprecated. Remove at some point.
    if (data.contains("font") && data.value("font").type() == QVariant::String) {
        QString font = data.value("font").toString();
        setFontFamily(Utils::fontFamily(font));
        setFontSize(Utils::fontSize(font));
    }

    if (data.contains("font") && data.value("font").type() == QVariant::Map) {
        QVariantMap font = data.value("font").toMap();
        if (font.contains("family"))
            setFontFamily(font.value("family").toString());

        if (font.contains("size") && font.value("size").type() == QVariant::Int)
            setFontSize(font.value("size").toInt());
        else if (font.contains("size"))
            setFontSize(Utils::fontSize(font.value("size").toString()));

        if (font.contains("weight") && font.value("weight").type() == QVariant::Int)
            setFontWeight(font.value("weight").toInt());
        else if (font.contains("weight"))
            setFontWeight(FontLibrary::fontWeightFromCss(font.value("weight").toString()));

        if (font.contains("style") && font.value("style").type() == QVariant::Int)
            setFontStyle(static_cast<QFont::Style>(font.value("style").toInt()));
        else if (font.contains("style"))
            setFontStyle(FontLibrary::fontStyleFromCss(font.value("style").toString()));
    }

    if (data.contains("placeholderText") && data.value("placeholderText").type() == QVariant::String)
        this->setPlaceholderText(data.value("placeholderText").toString());

    if (data.contains("placeholderTextColor") && data.value("placeholderTextColor").type() == QVariant::List)
        setPlaceholderTextColor(Utils::listToColor(data.value("placeholderTextColor").toList()));
}

void TextBox::filterLoadData(QVariantMap & data)
{
    Object::filterLoadData(data);
    data.remove("text");
}

void TextBox::init(const QString& text, const QVariantMap &data)
{
    mText = text;
    setTextColor(QColor(Qt::black));
    mTextRect = sceneRect();
    mPlaceholderText = "";
    mTextAlignment = Qt::AlignLeft | Qt::AlignTop;
    setType(GameObjectMetaType::TextBox);
    mFont.setFamily(Object::defaultFontFamily());
    mFont.setPixelSize(Object::defaultFontSize());

    initRect(data);
}

void TextBox::initRect(const QVariantMap & data)
{
    if (!data.contains("width") && !data.contains("height")) {
        setHeight(Scene::height()/3);
        setWidth(Scene::width());
    }

    if (!data.contains("x") && !data.contains("y")) {
        setX(0);
        setY(Scene::height()-(Scene::height()/3));
    }
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
        notify("text", mText);
    }
}

QColor TextBox::textColor() const
{
    return mTextColor;
}

void TextBox::setTextColor(const QColor& color)
{
    mTextColor = color;
    this->notify("textColor", Utils::colorToList(color));
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

QColor TextBox::placeholderTextColor() const {
    return mPlaceholderTextColor;
}

void TextBox::setPlaceholderTextColor(const QColor& color)
{
    if (mPlaceholderTextColor != color)  {
        mPlaceholderTextColor = color;
    }
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

    QRect rect(contentRect());
    QPen pen(currentColor());
    painter.save();
    painter.setFont(mFont);
    painter.setPen(pen);
    painter.drawText(rect, mTextAlignment | Qt::TextWordWrap, currentText());
    painter.restore();
}

QVariantMap TextBox::toJsonObject(bool internal) const
{
    QVariantMap object = Object::toJsonObject(internal);

    object.insert("textColor", Utils::colorToList(textColor()));
    object.insert("text", mText);
    object.insert("textAlignment", textAlignmentAsString());
    if (mFont != Object::defaultFont()) {
        QVariantMap font;
        QFontMetrics metrics(mFont);
        font.insert("size", QString("%1px").arg(mFont.pixelSize()));
        font.insert("family", mFont.family());
        if (metrics.leading())
            font.insert("leading", metrics.leading());
        font.insert("height", metrics.height());
        font.insert("weight", FontLibrary::cssFontWeight(mFont.weight()));
        font.insert("style", FontLibrary::cssFontStyle(mFont.style()));
        object.insert("font", font);
    }

    if (internal) {
        if (! placeholderText().isEmpty())
            object.insert("placeholderText", placeholderText());
        if (placeholderTextColor().isValid())
            object.insert("placeholderTextColor", placeholderTextColor());
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

QColor TextBox::currentColor() const
{
    if (! mPlaceholderText.isEmpty() && mPlaceholderTextColor.isValid())
        return mPlaceholderTextColor;
    return mTextColor;
}

Qt::Alignment TextBox::textAlignment()
{
    return mTextAlignment;
}

QString TextBox::textAlignmentAsString() const
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
    this->notify("textAlignment", textAlignmentAsString());
}

int TextBox::fontSize()
{
    return mFont.pixelSize();
}

void TextBox::setFontSize(int size)
{
    mFont.setPixelSize(size);
    notifyFont("size", mFont.pixelSize());
}

QString TextBox::fontFamily()
{
    return mFont.family();
}

void TextBox::setFontFamily(const QString& family)
{
    mFont.setFamily(family);
    QFont::StyleHint hint = FontLibrary::fontStyleHint(family);
    mFont.setStyleHint(hint);
    notifyFont("family", mFont.family());
}

void TextBox::notifyFont(const QString & property, const QVariant & value)
{
    QVariantMap font;
    font.insert(property, value);
    notify("font", font);
}

int TextBox::fontWeight() const
{
    return mFont.weight();
}

void TextBox::setFontWeight(int w)
{
    mFont.setWeight(w);
    notifyFont("weight", mFont.weight());
}

QFont::Style TextBox::fontStyle() const
{
    return mFont.style();
}

void TextBox::setFontStyle(QFont::Style s)
{
    mFont.setStyle(s);
    notifyFont("style", mFont.style());
}
