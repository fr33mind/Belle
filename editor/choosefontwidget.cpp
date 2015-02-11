#include "choosefontwidget.h"

#include <QFontDatabase>
#include <QStandardItemModel>
#include <QPainter>
#include <QFileDialog>
#include <QDebug>

#include "assetmanager.h"
#include "fontfile.h"

static QStringList mFontFamilies;

ChooseFontWidget::ChooseFontWidget(QWidget *parent) :
    QComboBox(parent)
{
    mCurrentIndex = 0;
    mCurrentFamily = "";
    loadFonts();
    connect(this, SIGNAL(activated(int)), this, SLOT(onItemActivated(int)));
}

void ChooseFontWidget::setCurrentFontFamily(const QString& family)
{
    mCurrentIndex = mFontFamilies.indexOf(family);
    mCurrentFamily = family;
    setCurrentIndex(mCurrentIndex);
}

void ChooseFontWidget::loadFonts()
{

    bool loadedCustom = false, loadedSystem = false;
    loadedCustom = loadCustomFonts();
    loadedSystem = loadSystemFonts();

    if (loadedCustom || loadedSystem)
        mFontFamilies.sort();

    if (mFontFamilies.size() != count()-1) {
        this->blockSignals(true);
        clear();
        addItems(mFontFamilies);
        addCustomFontItem();
        this->blockSignals(false);
    }
}

bool ChooseFontWidget::loadSystemFonts()
{
    if (mFontFamilies.isEmpty()) {
        QList<QFontDatabase::WritingSystem> writingSystems = mFontDatabase.writingSystems();
        QStringList families;
        for(int i=0; i < writingSystems.size(); i++) {
            families = mFontDatabase.families(writingSystems[i]);
            for(int j=0; j < families.size(); j++) {
                if (! mFontFamilies.contains(families[j]))
                    mFontFamilies.append(families[j]);
            }
        }
        return true;
    }

    return false;
}

bool ChooseFontWidget::loadCustomFonts()
{
    QList<int> fontsIds = AssetManager::instance()->fontsIds();
    if (fontsIds.size() != mCustomFontsIds.size()) {
        QStringList families;
        foreach(int id, fontsIds) {
            if (! mCustomFontsIds.contains(id)) {
                mCustomFontsIds.append(id);
                families = QFontDatabase::applicationFontFamilies(id);

                for(int i=0; i < families.size(); i++)
                    if (! mFontFamilies.contains(families[i]))
                        mFontFamilies.append(families[i]);
            }
        }
        return true;
    }

    return false;
}

void ChooseFontWidget::addCustomFontItem()
{
    addItem(QIcon(":/media/document_open.png"), tr("Custom Font"), "CustomFont");
}

void ChooseFontWidget::onItemActivated(int index)
{
    if (index == count()-1) {
        QString filePath = QFileDialog::getOpenFileName(0, tr("Choose a custom font"), QString(), QString("Fonts (*.ttf *.otf)"));
        QStringList families;

        if (! filePath.isEmpty()) {
            FontFile* fontFile = dynamic_cast<FontFile*>(AssetManager::instance()->loadAsset(filePath, Asset::Font));
            int id = fontFile->id();
            if (id != -1) {
                loadFonts();
                families = mFontDatabase.applicationFontFamilies(id);
            }
        }

        if (families.isEmpty())
            setCurrentIndex(mCurrentIndex);
        else {
            setCurrentFontFamily(families.first());
            emit fontChosen(mFontFamilies[currentIndex()]);
        }
    }
    else {
        mCurrentIndex = index;
        mCurrentFamily = currentText();
        emit fontChosen(mFontFamilies[currentIndex()]);
    }
}

void ChooseFontWidget::focusInEvent(QFocusEvent *e)
{
    QList<int> fonts = AssetManager::instance()->fontsIds();
    if (mCustomFontsIds.size() !=  fonts.size() || mFontFamilies.size() != count()-1) {
        loadFonts();
        setCurrentFontFamily(mCurrentFamily);
    }
    QComboBox::focusInEvent(e);
}
