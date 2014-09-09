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

#ifndef TEXT_PROPERTIES_WIDGET_H
#define TEXT_PROPERTIES_WIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QSpinBox>

#include "color_pushbutton.h"
#include "textbox.h"
#include "object.h"
#include "object_editor_widget.h"
#include "choosefontwidget.h"

class TextBox;

class TextPropertiesWidget : public ObjectEditorWidget
{
    Q_OBJECT

    QTextEdit* mTextEdit;
    ColorPushButton* mColorButton;
    QComboBox* mHorizontalAlignmentComboBox;
    QComboBox* mVerticalAlignmentComboBox;
    QSpinBox* mFontSizeSpin;
    ChooseFontWidget* mChooseFontWidget;


public:
    TextPropertiesWidget(QWidget *parent = 0);
    virtual void updateData(Object*);

signals:

public slots:
    void onTextEditDataChanged();
    void onColorChosen(const QColor&);

private slots:
    void onAlignmentChanged(int);
    void onFontSizeChanged(int);
    void onFontChosen(const QString&);

private:
    Qt::Alignment horizontalAlignment();
    Qt::Alignment verticalAlignment();

};

#endif // TEXT_PROPERTIES_WIDGET_H
