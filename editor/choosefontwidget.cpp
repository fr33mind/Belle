#include "choosefontwidget.h"

#include "fontlibrary.h"

ChooseFontWidget::ChooseFontWidget(QWidget *parent) :
    QComboBox(parent)
{
    loadFonts();
}

void ChooseFontWidget::loadFonts()
{
    clear();
    loadApplicationFonts();
    loadWebSafeFonts();
}

void ChooseFontWidget::loadWebSafeFonts()
{
    if (count())
        insertSeparator(count());

    QStringList fonts = FontLibrary::webSafeFonts();
    fonts.sort();
    addItems(fonts);
}

void ChooseFontWidget::loadApplicationFonts()
{
    QStringList fonts = FontLibrary::applicationFontFamilies();
    fonts.sort();
    addItems(fonts);
}
