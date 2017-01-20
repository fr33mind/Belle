#include "multisourceasset.h"

#include "assetmanager.h"

MultiSourceAsset::MultiSourceAsset(MultiSourceAsset::Type type) :
    Asset("", (Asset::Type) type)
{
    init();
}

MultiSourceAsset::MultiSourceAsset(const QString& path, MultiSourceAsset::Type type) :
    Asset("", (Asset::Type) type)
{
    init();
    addSource(path);
}

MultiSourceAsset::MultiSourceAsset(const QVariantMap& data, MultiSourceAsset::Type type) :
    Asset("", (Asset::Type) type)
{
    init();

    if (data.value("name").type() == QVariant::String) {
        setName(data.value("name").toString());
    }

    if (data.value("sources").type() == QVariant::List) {
        QVariantList sources = data.value("sources").toList();
        QVariantMap srcData;
        foreach(const QVariant& src, sources) {
            if (src.type() == QVariant::Map) {
                srcData = src.toMap();
                addSource(srcData.value("name").toString());
            }
        }
    }
}

MultiSourceAsset::~MultiSourceAsset()
{
    for(int i=mSources.size()-1; i >= 0; i--) {
        removeSourceAt(i);
    }

    mSources.clear();
}

void MultiSourceAsset::init()
{
    setName("asset");
}

bool MultiSourceAsset::isValid() const
{
    return !isNull();
}

bool MultiSourceAsset::isNull() const
{
    return mSources.isEmpty();
}

Asset* MultiSourceAsset::addSource(const QString & src)
{
    if (containsSource(src))
        return source(src);

    Asset* asset = AssetManager::instance()->loadAsset(src, sourceType(type()));
    if (asset)
        mSources.append(asset);
    return asset;
}

void MultiSourceAsset::addSource(Asset* asset)
{
    if (!asset || mSources.contains(asset))
        return;

    AssetManager::instance()->loadAsset(asset->path(), sourceType(type()));
    mSources.append(asset);
}

Asset* MultiSourceAsset::sourceAt(int index) const
{
    if (index >= 0 && index < mSources.size())
        return mSources.at(index);
    return 0;
}


void MultiSourceAsset::removeSourceAt(int index)
{
    if (index >= 0 && index < mSources.size()) {
        Asset* asset = mSources.takeAt(index);
        sourceRemoved(asset);
        AssetManager::instance()->releaseAsset(asset);
    }
}

bool MultiSourceAsset::removeSource(Asset* asset)
{
    int index = mSources.indexOf(asset);
    if (index != -1) {
        removeSourceAt(index);
        return true;
    }
    return false;
}

QList<Asset*> MultiSourceAsset::sources() const
{
    return mSources;
}

Asset::Type MultiSourceAsset::sourceType(Asset::Type type)
{
    switch(type) {
    case Asset::Audio: return Asset::AudioSource;
    case Asset::Video: return Asset::VideoSource;
    case Asset::Font: return Asset::FontSource;
    default: break;
    }

    return Asset::Unknown;
}

bool MultiSourceAsset::save(const QDir& dir, bool updatePath)
{
    if (!isValid())
        return false;

    bool success = false,
         successAny = false;

    foreach(Asset* src, mSources) {
        success = src->save(dir, updatePath);
        if (success)
            successAny = true;
    }

    return successAny;
}

bool MultiSourceAsset::doSave(const QDir & dir)
{
    return false;
}

bool MultiSourceAsset::remove()
{
    if (!isRemovable())
        return false;

    bool success = false,
         successAny = false;

    foreach(Asset* src, mSources) {
        src->setRemovable(true);
        success = src->remove();
        if (success)
            successAny = true;
    }

    return successAny;
}

Asset* MultiSourceAsset::source(const QString & path)
{
    foreach(Asset* src, mSources) {
        if (src->path() == path)
            return src;
    }

    return 0;
}

bool MultiSourceAsset::containsSource(const QString & path)
{
    if (source(path))
        return true;
    return false;
}

QVariantMap MultiSourceAsset::toJsonObject()
{
    QVariantMap data = Asset::toJsonObject();
    QVariantList sources;

    foreach(Asset* src, mSources) {
        sources.append(src->toJsonObject());
    }

    data.insert("sources", sources);
    return data;
}

void MultiSourceAsset::sourceRemoved(Asset * asset)
{
}
