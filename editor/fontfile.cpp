#include "fontfile.h"

#include <QFontDatabase>

FontFile::FontFile(const QString& path) :
    Asset(path, Asset::FontSource)
{
    mId = QFontDatabase::addApplicationFont(path);
}

FontFile::~FontFile()
{
    QFontDatabase::removeApplicationFont(mId);
}

int FontFile::id() const
{
    return mId;
}

bool FontFile::isNull() const
{
    return mId == -1;
}

QStringList FontFile::fontFamilies() const
{
    if (mId != -1)
        return QFontDatabase::applicationFontFamilies(mId);

    return QStringList();
}

QString FontFile::fontFamily() const
{
    QStringList families = fontFamilies();
    if (!families.isEmpty())
        return families.first();
    return "";
}
