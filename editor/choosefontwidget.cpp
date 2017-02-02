#include "choosefontwidget.h"

#include <QMessageBox>
#include <QCheckBox>

#include "fontlibrary.h"

static bool mShowWebSafeFontsMessage = true;

ChooseFontWidget::ChooseFontWidget(QWidget *parent) :
    QComboBox(parent)
{
    loadFonts();
    connect(this, SIGNAL(activated(int)), this, SLOT(onActivated(int)));
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
    foreach(const QString& font, fonts) {
        addItem(font, "websafe");
    }
}

void ChooseFontWidget::loadApplicationFonts()
{
    QStringList fonts = FontLibrary::applicationFontFamilies();
    fonts.sort();
    addItems(fonts);
}

void ChooseFontWidget::onActivated(int index)
{
    if (mShowWebSafeFontsMessage && itemData(index).toString() == "websafe") {
        QString title = tr("Web-safe Font");
        QString msg = tr("You just chose a web-safe font. This font is only referenced by name, "
                         "therefore the availability of the font depends on the system running your game.\n"
                         "Despite of this, generally they are safe to use because they're likely to be present on many systems. "
                         "Even if they're not present, usually browsers can find similar alternatives.\n"
                         "However if you want to be sure everyone uses the same font, add a font file through the Resources section.");

        QMessageBox msgBox(QMessageBox::Information, title, msg, QMessageBox::Ok, this);
        QCheckBox checkBox(tr("Do not show this again"));
        msgBox.setCheckBox(&checkBox);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        setShowWebSafeFontsMessage(!checkBox.isChecked());
    }
}

bool ChooseFontWidget::showWebSafeFontsMessage()
{
    return mShowWebSafeFontsMessage;
}

void ChooseFontWidget::setShowWebSafeFontsMessage(bool show)
{
    mShowWebSafeFontsMessage = show;
}
