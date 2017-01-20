#include "fontasset.h"

#include "fontfile.h"
#include "fontlibrary.h"

FontAsset::FontAsset() :
    MultiSourceAsset(MultiSourceAsset::Font)
{
    init();
}

FontAsset::FontAsset(const QString& path) :
    MultiSourceAsset(path, MultiSourceAsset::Font)
{
    init();
    FontFile* fontfile = mainFontFile();
    if (fontfile) {
        setFontFamily(fontfile->fontFamily());
        setFontId(fontfile->id());
    }
}

FontAsset::FontAsset(const QVariantMap& data) :
    MultiSourceAsset(data, MultiSourceAsset::Font)
{
    init();

    if (data.value("fontFamily").type() == QVariant::String) {
        setFontFamily(data.value("fontFamily").toString());
    }

    if (data.value("fontWeight").type() == QVariant::String) {
        setFontWeight(data.value("fontWeight").toString());
    }

    if (data.value("fontStyle").type() == QVariant::String) {
        setFontStyle(data.value("fontStyle").toString());
    }

    if (data.value("fallback").type() == QVariant::String) {
        setFallback(data.value("fallback").toString());
    }

    if (data.value("genericFontFamily").type() == QVariant::String) {
        setGenericFontFamily(data.value("genericFontFamily").toString());
    }

    FontFile* fontfile = mainFontFile();
    if (fontfile) {
        setFontFamily(fontfile->fontFamily());
        setFontId(fontfile->id());
    }
}

FontAsset::~FontAsset()
{
    FontLibrary::removeFontSubstitutions(fontFamily());
}

void FontAsset::init()
{
    setFontWeight("normal");
    setFontStyle("normal");
    mFontId = -1;
}

FontFile* FontAsset::mainFontFile() const
{
    Asset* src = sourceAt(0);
    if (src) {
        return static_cast<FontFile*>(src);
    }

    return 0;
}

int FontAsset::fontId() const
{
    return mFontId;
}

void FontAsset::setFontId(int id)
{
    mFontId = id;
}

QString FontAsset::fontFamily() const
{
    return mFontFamily;
}

void FontAsset::setFontFamily(const QString & family)
{
    if (mFontFamily == family)
        return;

    FontLibrary::removeFontSubstitutions(mFontFamily);
    mFontFamily = family;
    FontLibrary::insertFontSubstitutions(mFontFamily, fallbacks());
}

QString FontAsset::fontWeight() const
{
    return mFontWeight;
}

void FontAsset::setFontWeight(const QString & weight)
{
    mFontWeight = weight;
}

QString FontAsset::fontStyle() const
{
    return mFontStyle;
}

void FontAsset::setFontStyle(const QString & style)
{
    mFontStyle = style;
}

QString FontAsset::fallback() const
{
    return mFallback;
}

QStringList FontAsset::fallbacks() const
{
    QStringList fallbacks;
    QStringList parts = mFallback.split(",", QString::SkipEmptyParts);
    foreach(const QString& part, parts) {
        if (!part.trimmed().isEmpty())
            fallbacks.append(part.trimmed());
    }
    return fallbacks;
}

void FontAsset::setFallback(const QString & fallback)
{
    mFallback = fallback;
    FontLibrary::removeFontSubstitutions(mFontFamily);
    FontLibrary::insertFontSubstitutions(mFontFamily, fallbacks());
}

QString FontAsset::genericFontFamily() const
{
    return mGenericFontFamily;
}

void FontAsset::setGenericFontFamily(const QString & family)
{
    mGenericFontFamily = family;
}

QVariantMap FontAsset::toJsonObject()
{
    QVariantMap data = MultiSourceAsset::toJsonObject();

    data.insert("fontFamily", mFontFamily);
    data.insert("fontWeight", mFontWeight);
    data.insert("fontStyle", mFontStyle);
    data.insert("fallback", mFallback);
    data.insert("genericFontFamily", mGenericFontFamily);

    return data;
}

QString FontAsset::toFontFace() const
{
    QStringList lines;

    lines << "@font-face {"
          << QString("font-family: '%1';").arg(mFontFamily)
          << QString("src: %1;").arg(fontFaceUrls())
          << QString("font-weight: %1;").arg(mFontWeight)
          << QString("font-style: %1;").arg(mFontStyle)
          << "}";

    return lines.join("\n");
}

QString FontAsset::fontFaceUrls() const
{
    QList<Asset*> srcs = sources();
    QString suffix;
    QStringList urls;
    QString eot;

    foreach(Asset* src, srcs) {
        if (!src)
            continue;

        suffix = QFileInfo(src->path()).suffix().toLower();
        if (suffix == "eot")
            eot = QString("url('%1?#iefix') format('embedded-opentype')").arg(src->name());
        else if (suffix == "ttf")
            urls.append(QString("url('%1') format('truetype')").arg(src->name()));
        else if (suffix == "otf")
            urls.append(QString("url('%1') format('opentype')").arg(src->name()));
        else if (suffix == "woff")
            urls.append(QString("url('%1') format('woff')").arg(src->name()));
    }

    if (!eot.isEmpty())
        urls.prepend(eot);

    return urls.join(",\n");
}

QStringList FontAsset::supportedFormats()
{
    QStringList formats;
    formats << "ttf" << "otf" << "woff" << "eot";
    return formats;
}

void FontAsset::sourceRemoved(Asset * asset)
{
    FontFile* fontSource = static_cast<FontFile*>(asset);
    if (!fontSource)
        return;

    if (mFontId == fontSource->id()) {
        setFontId(-1);
        Asset* source = sourceAt(0);
        if(source) {
            fontSource = static_cast<FontFile*>(source);
            setFontId(fontSource->id());
            setFontFamily(fontSource->fontFamily());
        }
    }
}
