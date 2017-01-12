#include "fontlibrary.h"

#include <QObject>
#include <QFontDatabase>
#include <QStringList>

#include "assetmanager.h"
#include "fontasset.h"

static QList<int> mFontWeights;
static QStringList mFontWeightNames;
static QStringList mCssFontWeights;
static QList<QFont::Style> mFontStyles;
static QStringList mFontStyleNames;
static QStringList mCssFontStyles;
static QList<QFont::StyleHint> mGenericFontFamilies;
static QStringList mGenericFontFamilyNames;
static QStringList mCssGenericFontFamilies;
static QStringList mWebSafeFonts;
static QHash<QString, QFont::StyleHint> mWebSafeFontStyleHints;

void FontLibrary::initFontProperties()
{
    mFontWeights << QFont::Light
                 << QFont::Normal
                 << QFont::DemiBold
                 << QFont::Bold
                 << QFont::Black;

    mFontWeightNames << QObject::tr("Light")
                     << QObject::tr("Normal")
                     << QObject::tr("Demi Bold")
                     << QObject::tr("Bold")
                     << QObject::tr("Black");

    mCssFontWeights  << "300"
                     << "normal"
                     << "600"
                     << "bold"
                     << "900";

    mFontStyles  << QFont::StyleNormal
                 << QFont::StyleItalic
                 << QFont::StyleOblique;

    mFontStyleNames  << QObject::tr("Normal")
                     << QObject::tr("Italic")
                     << QObject::tr("Oblique");

    mCssFontStyles << "normal"
                   << "italic"
                   << "oblique";

    mGenericFontFamilies << QFont::SansSerif
                         << QFont::Serif
                         << QFont::Cursive
                         << QFont::Fantasy
                         << QFont::Monospace;

    mGenericFontFamilyNames  << QObject::tr("sans-serif")
                             << QObject::tr("serif")
                             << QObject::tr("cursive")
                             << QObject::tr("fantasy")
                             << QObject::tr("monospace");

    mCssGenericFontFamilies << "sans-serif"
                            << "serif"
                            << "cursive"
                            << "fantasy"
                            << "monospace";
}

void FontLibrary::initWebSafeFonts()
{
    QStringList fonts;
    fonts << "Georgia,serif";
    fonts << "Palatino Linotype,Book Antiqua,Palatino,serif";
    fonts << "Times New Roman,Times,serif";
    fonts << "Arial,Helvetica,sans-serif";
    fonts << "Arial Black,Gadget,sans-serif";
    fonts << "Comic Sans MS,cursive,sans-serif";
    fonts << "Impact,Charcoal,sans-serif";
    fonts << "Lucida Sans Unicode,Lucida Grande,sans-serif";
    fonts << "Tahoma,Geneva,sans-serif";
    fonts << "Trebuchet MS,Helvetica,sans-serif";
    fonts << "Verdana,Geneva,sans-serif";
    fonts << "Courier New,Courier,monospace";
    fonts << "Lucida Console,Monaco,monospace";

    QStringList _fonts;
    QString _font;
    foreach(const QString& font, fonts) {
        _fonts = font.split(",");
        if (!_fonts.isEmpty()) {
            _font = _fonts.first();
            QFont::insertSubstitutions(_font, _fonts.mid(1));
            mWebSafeFonts.append(_font);
            mWebSafeFontStyleHints.insert(_font, genericFontFamilyFromCss(_fonts.last()));
        }
    }
}

void FontLibrary::init()
{
    initFontProperties();
    initWebSafeFonts();
}

QStringList FontLibrary::webSafeFonts()
{
    return mWebSafeFonts;
}

QList<int> FontLibrary::fontWeights()
{
    return mFontWeights;
}

QStringList FontLibrary::cssFontWeights()
{
    return mCssFontWeights;
}

int FontLibrary::fontWeightFromCss(const QString& weight)
{
    int index = mCssFontWeights.indexOf(weight.toLower());
    if (index >= 0 && index < mFontWeights.size())
        return mFontWeights.at(index);
    return -1;
}

QString FontLibrary::cssFontWeight(int weight)
{
    int index = mFontWeights.indexOf(weight);
    if (index >= 0 && index < mCssFontWeights.size())
        return mCssFontWeights.at(index);
    return "";
}

QString FontLibrary::fontWeightName(int weight)
{
    int index = mFontWeights.indexOf(weight);
    if (index >= 0 && index < mFontWeightNames.size())
        return mFontWeightNames.at(index);
    return "";
}

QString FontLibrary::fontWeightNameFromCss(const QString& weight)
{
    int w = fontWeightFromCss(weight);
    return fontWeightName(w);
}

QList<QFont::Style> FontLibrary::fontStyles()
{
    return mFontStyles;
}

QStringList FontLibrary::cssFontStyles()
{
    return mCssFontStyles;
}

QFont::Style FontLibrary::fontStyleFromCss(const QString& style)
{
    int index = mCssFontStyles.indexOf(style.toLower());
    if (index >= 0 && index < mFontStyles.size())
        return mFontStyles.at(index);
    return QFont::StyleNormal;
}

QString FontLibrary::cssFontStyle(QFont::Style style)
{
    int index = mFontStyles.indexOf(style);
    if (index >= 0 && index < mCssFontStyles.size())
        return mCssFontStyles.at(index);
    return "";
}

QString FontLibrary::fontStyleName(QFont::Style style)
{
    int index = mFontStyles.indexOf(style);
    if (index >= 0 && index < mFontStyleNames.size())
        return mFontStyleNames.at(index);
    return "";
}

QString FontLibrary::fontStyleNameFromCss(const QString& style)
{
    QFont::Style s = fontStyleFromCss(style);
    return fontStyleName(s);
}

QList<QFont::StyleHint> FontLibrary::genericFontFamilies()
{
    return mGenericFontFamilies;
}

QStringList FontLibrary::cssGenericFontFamilies()
{
    return mCssGenericFontFamilies;
}

QFont::StyleHint FontLibrary::genericFontFamilyFromCss(const QString& style)
{
    int index = mCssGenericFontFamilies.indexOf(style.toLower());
    if (index >= 0 && index < mGenericFontFamilies.size())
        return mGenericFontFamilies.at(index);
    return QFont::AnyStyle;
}

QString FontLibrary::cssGenericFontFamily(QFont::StyleHint style)
{
    int index = mGenericFontFamilies.indexOf(style);
    if (index >= 0 && index < mCssGenericFontFamilies.size())
        return mCssGenericFontFamilies.at(index);
    return "";
}

QString FontLibrary::genericFontFamilyName(QFont::StyleHint style)
{
    int index = mGenericFontFamilies.indexOf(style);
    if (index >= 0 && index < mGenericFontFamilyNames.size())
        return mGenericFontFamilyNames.at(index);
    return "";
}

QString FontLibrary::genericFontFamilyNameFromCss(const QString& style)
{
    QFont::StyleHint f = genericFontFamilyFromCss(style);
    return genericFontFamilyName(f);
}

QFont::StyleHint FontLibrary::fontStyleHint(const QString & family)
{
    QList<Asset*> assets = AssetManager::instance()->assets(Asset::Font);
    FontAsset* fontAsset = 0;

    foreach(Asset* asset, assets) {
        fontAsset = static_cast<FontAsset*>(asset);
        if (fontAsset->fontFamily().toLower() == family.toLower()) {
            return genericFontFamilyFromCss(fontAsset->genericFontFamily());
        }
    }

    QHashIterator<QString, QFont::StyleHint> it(mWebSafeFontStyleHints);
    while(it.hasNext()) {
        it.next();
        if (it.key().toLower() == family.toLower())
            return it.value();
    }

    return QFont::AnyStyle;
}

QStringList FontLibrary::applicationFontFamilies()
{
    QList<int> fontsIds = AssetManager::instance()->fontsIds();
    QStringList families, appFamilies;

    foreach(int id, fontsIds) {
        families = QFontDatabase::applicationFontFamilies(id);
        if (!families.isEmpty())
            appFamilies.append(families.first());
    }

    appFamilies.removeDuplicates();

    return appFamilies;
}

void FontLibrary::insertFontSubstitutions(const QString & family, const QStringList & substitutes)
{
    QFont::insertSubstitutions(family, substitutes);
}

void FontLibrary::insertFontSubstitution(const QString & family, const QString & substitute)
{
    QFont::insertSubstitution(family, substitute);
}

QStringList FontLibrary::fontSubstitutes(const QString & family)
{
    return QFont::substitutes(family);
}

void FontLibrary::removeFontSubstitutions(const QString & family)
{
    QFont::removeSubstitutions(family);
}

bool FontLibrary::removeFontSubstitute(const QString & family, const QString & substitute)
{
    QStringList substitutes = FontLibrary::fontSubstitutes(family);
    //Substitutions are inserted as lower case, so we need to put the substitute as lower as well.
    int removed = substitutes.removeAll(substitute.toLower());
    if (!removed)
        return false;

    FontLibrary::removeFontSubstitutions(family);
    FontLibrary::insertFontSubstitutions(family, substitutes);
    return true;
}
