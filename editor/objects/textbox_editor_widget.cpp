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

#include "textbox_editor_widget.h"

#include <QTextEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QDebug>

#include "fontlibrary.h"

TextPropertiesWidget::TextPropertiesWidget(QWidget *parent) :
    ObjectEditorWidget(parent)
{

    mColorButton = new ColorPushButton(this);

    QColor color = this->palette().color(QPalette::WindowText);
    mColorButton->setText(color.name());
    mTextEdit = new QTextEdit(this);
    mChooseFontWidget = new ChooseFontWidget(this);
    mFontSizeSpin = new QSpinBox(this);
    mHorizontalAlignmentComboBox = new QComboBox(this);
    mHorizontalAlignmentComboBox->addItem(tr("Center"), "hcenter");
    mHorizontalAlignmentComboBox->addItem(tr("Right"), "right");
    mHorizontalAlignmentComboBox->addItem(tr("Left"), "left");
    mVerticalAlignmentComboBox = new QComboBox(this);
    mVerticalAlignmentComboBox->addItem(tr("Center"), "vcenter");
    mVerticalAlignmentComboBox->addItem(tr("Bottom"), "bottom");
    mVerticalAlignmentComboBox->addItem(tr("Top"), "top");
    mFontWeightsComboBox = new QComboBox(this);
    setupFontWeights(mFontWeightsComboBox);
    mFontStylesComboBox = new QComboBox(this);
    setupFontStyles(mFontStylesComboBox);

    beginGroup("Text");
    appendRow(tr("Text"), mTextEdit);
    appendRow(tr("Color"), mColorButton);
    appendRow(tr("Font"), mChooseFontWidget);
    appendRow(tr("Font size"), mFontSizeSpin);
    appendRow(tr("Font weight"), mFontWeightsComboBox);
    appendRow(tr("Font style"), mFontStylesComboBox);
    appendRow(tr("Horizontal Alignment"), mHorizontalAlignmentComboBox);
    appendRow(tr("Vertical Alignment"), mVerticalAlignmentComboBox);
    endGroup();

    connect(mTextEdit, SIGNAL(textChanged()), this, SLOT(onTextEditDataChanged()));
    connect(mColorButton, SIGNAL(colorChosen(const QColor&)), this, SLOT(onColorChosen(const QColor&)));
    connect(mHorizontalAlignmentComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAlignmentChanged(int)));
    connect(mVerticalAlignmentComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onAlignmentChanged(int)));
    connect(mFontSizeSpin, SIGNAL(valueChanged(int)), this, SLOT(onFontSizeChanged(int)));
    connect(mChooseFontWidget, SIGNAL(currentTextChanged(const QString&)), this, SLOT(onFontChosen(const QString&)));
    connect(mFontWeightsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onFontWeightChanged(int)));
    connect(mFontStylesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onFontStyleChanged(int)));

    mTextEdit->setMaximumHeight(mTextEdit->height()/2);
}


void TextPropertiesWidget::onTextEditDataChanged()
{
    TextBox* textbox = qobject_cast<TextBox*>(mGameObject);
    if (textbox)
        textbox->setText(mTextEdit->toPlainText());
}

void TextPropertiesWidget::onColorChosen(const QColor& color)
{
    TextBox* textbox = qobject_cast<TextBox*>(mGameObject);
    if (textbox)
        textbox->setTextColor(color);
}

void TextPropertiesWidget::updateData(GameObject* obj)
{
    ObjectEditorWidget::updateData(obj);
    TextBox* textbox = qobject_cast<TextBox*>(obj);

    if (! textbox)
        return;

    if (! textbox->text().isEmpty())
        textbox->setPlaceholderText("");

    mChooseFontWidget->loadFonts();

    mHorizontalAlignmentComboBox->setCurrentIndex(mHorizontalAlignmentComboBox->count()-1);
    mVerticalAlignmentComboBox->setCurrentIndex(mVerticalAlignmentComboBox->count()-1);

    QStringList alignment = textbox->textAlignmentAsString().toLower().split("|");

    if (alignment.size() >= 1) {
        int index = mHorizontalAlignmentComboBox->findData(alignment[0], Qt::UserRole, Qt::MatchFixedString);
        if (index >= 0)
            mHorizontalAlignmentComboBox->setCurrentIndex(index);
    }

    if (alignment.size() >= 2) {
        int index = mVerticalAlignmentComboBox->findData(alignment[1], Qt::UserRole, Qt::MatchFixedString);
        if (index >= 0)
            mVerticalAlignmentComboBox->setCurrentIndex(index);
    }

    mTextEdit->setText(textbox->text());
    mColorButton->setColor(textbox->textColor());
    mChooseFontWidget->setCurrentText(textbox->fontFamily());
    mFontSizeSpin->setValue(textbox->fontSize());
    int index = mFontWeightsComboBox->findData(textbox->fontWeight());
    mFontWeightsComboBox->setCurrentIndex(index);

    index = mFontStylesComboBox->findData(textbox->fontStyle());
    mFontStylesComboBox->setCurrentIndex(index);
}

void TextPropertiesWidget::onAlignmentChanged(int index)
{
    QComboBox* senderCombo = static_cast<QComboBox*>(this->sender());
    TextBox* textbox = qobject_cast<TextBox*>(mGameObject);

    if (index < 0 || index >= senderCombo->count() || ! textbox)
        return;

    Qt::Alignment alignment = horizontalAlignment();
    alignment |= verticalAlignment();
    textbox->setTextAlignment(alignment);
}

Qt::Alignment TextPropertiesWidget::horizontalAlignment()
{
    Qt::Alignment alignment;

    QVariant data = mHorizontalAlignmentComboBox->itemData(mHorizontalAlignmentComboBox->currentIndex());
    QString align = data.toString();

    if (align == "hcenter")
        alignment = Qt::AlignHCenter;
    else if (align == "right")
        alignment = Qt::AlignRight;
    else
        alignment = Qt::AlignLeft;

    return alignment;
}


Qt::Alignment TextPropertiesWidget::verticalAlignment()
{
    Qt::Alignment alignment;

    QVariant data = mVerticalAlignmentComboBox->itemData(mVerticalAlignmentComboBox->currentIndex());
    QString align = data.toString();

    if (align == "vcenter")
        alignment = Qt::AlignVCenter;
    else if (align == "bottom")
        alignment = Qt::AlignBottom;
    else
        alignment = Qt::AlignTop;

    return alignment;
}

void TextPropertiesWidget::onFontSizeChanged(int size)
{
    TextBox* textbox = qobject_cast<TextBox*>(mGameObject);
    if (textbox)
        textbox->setFontSize(size);
}

void TextPropertiesWidget::onFontChosen(const QString & family)
{
    TextBox* textbox = qobject_cast<TextBox*>(mGameObject);
    if (textbox)
        textbox->setFontFamily(family);
}

void TextPropertiesWidget::setupFontWeights(QComboBox * comboBox)
{
    QList<int> weights = FontLibrary::fontWeights();

    comboBox->clear();
    foreach(int weight, weights) {
        comboBox->addItem(FontLibrary::fontWeightName(weight), weight);
    }
}

void TextPropertiesWidget::onFontWeightChanged(int index)
{
    TextBox* textbox = qobject_cast<TextBox*>(mGameObject);
    if (textbox) {
        QVariant data = mFontWeightsComboBox->itemData(index);
        textbox->setFontWeight(data.toInt());
    }
}

void TextPropertiesWidget::setupFontStyles(QComboBox * comboBox)
{
    QList<QFont::Style> styles = FontLibrary::fontStyles();

    comboBox->clear();
    foreach(QFont::Style style, styles) {
        comboBox->addItem(FontLibrary::fontStyleName(style), style);
    }
}

void TextPropertiesWidget::onFontStyleChanged(int index)
{
    TextBox* textbox = qobject_cast<TextBox*>(mGameObject);
    if (textbox) {
        QVariant data = mFontStylesComboBox->itemData(index);
        textbox->setFontStyle(static_cast<QFont::Style>(data.toInt()));
    }
}
