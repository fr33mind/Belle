#include "asset.h"

#include <QFileInfo>

Asset::Asset(const QString& path, Type type)
{
    init();
    mPath = path;
    mType = type;
    mName = QFileInfo(path).fileName();
}

void Asset::init()
{
    mType = Unknown;
    mPath = "";
    mName = "";
}

void Asset::setName(const QString& name)
{
    mName = name;
}

QString Asset::name() const
{
    return mName;
}

Asset::Type Asset::type() const
{
    return mType;
}

QString Asset::path() const
{
    return mPath;
}

bool Asset::isValid() const
{
    if (!mName.isEmpty() && QFile::exists(mPath))
        return true;
    return false;
}

void Asset::save(const QDir& dir)
{
    if (isValid())
        QFile::copy(mPath, dir.absoluteFilePath(mName));
}

QVariantMap Asset::toJsonObject()
{
    QVariantMap data;
    data.insert("name", mName);
    return data;
}

