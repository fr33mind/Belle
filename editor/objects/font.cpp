#include "font.h"

#include "assetmanager.h"

Font::Font(QObject *parent) :
    GameObject(parent)
{
    init();
}

Font::Font(const QString& path, QObject *parent) :
    GameObject(parent)
{
    init();
    setFile(path);

    if (mFontAsset)
        setName(mFontAsset->fontFamily());
    else
        setName(tr("Font"));
}

Font::Font(const QVariantMap& data, QObject *parent) :
    GameObject(data, parent)
{
    init();
    loadInternal(data);
}

Font::~Font()
{
    AssetManager::instance()->releaseAsset(mFontAsset);
}

void Font::init()
{
    setType(GameObjectMetaType::Font);
    mFontAsset = 0;
}

void Font::loadData(const QVariantMap& data, bool internal)
{
    if (!internal)
        GameObject::loadData(data, internal);

    if (data.value("asset").type() == QVariant::String) {
        setFile(data.value("asset").toString());
    }
}

Asset* Font::setFile(const QString& path)
{
    if (mFontAsset)
        AssetManager::instance()->releaseAsset(mFontAsset);

    Asset* asset = AssetManager::instance()->loadAsset(path, Asset::Font);
    mFontAsset = dynamic_cast<FontAsset*>(asset);
    if (mFontAsset)
        return mFontAsset->sourceAt(0);

    return asset;
}

Asset* Font::addFile(const QString& path)
{
    if (mFontAsset) {
        return mFontAsset->addSource(path);
    }

    return setFile(path);
}

FontAsset* Font::asset() const
{
    return mFontAsset;
}

QVariantMap Font::toJsonObject(bool internal) const
{
    QVariantMap data = GameObject::toJsonObject(internal);
    if (mFontAsset)
        data.insert("asset", mFontAsset->name());
    return data;
}
