#include "asset.h"

#include <QFileInfo>

Asset::Asset(const QString& path, Type type)
{
    init();
    mRemovable = false;
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
    return !isNull() && QFile::exists(mPath);
}

bool Asset::isNull() const
{
    return mName.isEmpty() || mPath.isEmpty();
}

bool Asset::save(const QDir& dir, bool updatePath)
{
    bool success = onSave(dir);
    if (success && updatePath) {
        mPath = dir.absoluteFilePath(mName);
    }
    return success;
}

bool Asset::onSave(const QDir & dir)
{
    if (isValid())
        return QFile::copy(mPath, dir.absoluteFilePath(mName));
    return false;
}

QVariantMap Asset::toJsonObject()
{
    QVariantMap data;
    data.insert("name", mName);
    return data;
}

bool Asset::isRemovable() const
{
    return mRemovable && QFile::exists(mPath);
}

void Asset::setRemovable(bool removable)
{
    mRemovable = removable;
}

bool Asset::remove()
{
    if (isRemovable())
        return QFile::remove(mPath);
    return false;
}
