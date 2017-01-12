#ifndef FONTLIBRARY_H
#define FONTLIBRARY_H

#include <QHash>
#include <QFont>
#include <QStringList>

class FontLibrary
{
public:
    static void init();
    static QList<int> fontWeights();
    static QStringList cssFontWeights();

    static int fontWeightFromCss(const QString&);
    static QString cssFontWeight(int);

    static QString fontWeightName(int);
    static QString fontWeightNameFromCss(const QString&);

    static QList<QFont::Style> fontStyles();
    static QStringList cssFontStyles();

    static QFont::Style fontStyleFromCss(const QString&);
    static QString cssFontStyle(QFont::Style);

    static QString fontStyleName(QFont::Style);
    static QString fontStyleNameFromCss(const QString&);

    static QList<QFont::StyleHint> genericFontFamilies();
    static QStringList cssGenericFontFamilies();

    static QFont::StyleHint genericFontFamilyFromCss(const QString&);
    static QString cssGenericFontFamily(QFont::StyleHint);

    static QString genericFontFamilyName(QFont::StyleHint);
    static QString genericFontFamilyNameFromCss(const QString&);

    static QStringList webSafeFonts();
    static QFont::StyleHint fontStyleHint(const QString&);

    static QStringList applicationFontFamilies();

    static void insertFontSubstitutions(const QString&, const QStringList&);
    static void insertFontSubstitution(const QString&, const QString&);
    static QStringList fontSubstitutes(const QString&);
    static void removeFontSubstitutions(const QString&);
    static bool removeFontSubstitute(const QString&, const QString&);

private:
    static void initFontProperties();
    static void initWebSafeFonts();

};

#endif // FONTLIBRARY_H
