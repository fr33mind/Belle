#include "fontfile.h"

#include <QFontDatabase>

FontFile::FontFile(const QString& path) :
    Asset(path, Asset::Font)
{
    mId = QFontDatabase::addApplicationFont(path);
}

int FontFile::id() const
{
    return mId;
}
