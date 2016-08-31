#include "sound.h"

#include <QFileInfo>

#include "assetmanager.h"

Sound::Sound(QObject *parent) :
    GameObject(parent)
{
    init();
}

Sound::Sound(const QString& path, QObject *parent) :
    GameObject(parent)
{
    init();
    setFile(path);
    setName(QFileInfo(path).baseName());
}

Sound::Sound(const QVariantMap& data, QObject *parent) :
    GameObject(data, parent)
{
    init();
    _load(data);
}

Sound::~Sound()
{
    AssetManager::instance()->releaseAsset(mSoundAsset);
}

void Sound::init()
{
    setType(GameObjectMetaType::Sound);
    mSoundAsset = 0;
}

void Sound::_load(const QVariantMap& data)
{
    this->blockNotifications(true);

    if (data.value("asset").type() == QVariant::String) {
        setFile(data.value("asset").toString());
    }

    this->blockNotifications(false);
}

void Sound::load(const QVariantMap& data)
{
    GameObject::load(data);
    _load(data);
}

Asset* Sound::setFile(const QString& path)
{
    if (mSoundAsset)
        AssetManager::instance()->releaseAsset(mSoundAsset);

    Asset* asset = AssetManager::instance()->loadAsset(path, Asset::Audio);
    mSoundAsset = dynamic_cast<SoundAsset*>(asset);
    if (mSoundAsset)
        return mSoundAsset->sourceAt(0);

    return asset;
}

Asset* Sound::addFile(const QString& path)
{
    if (mSoundAsset) {
        return mSoundAsset->addSource(path);
    }

    return setFile(path);
}

Asset* Sound::assetAt(int index) const
{
    if (mSoundAsset)
        return mSoundAsset->sourceAt(index);
    return 0;
}

void Sound::removeAssetAt(int index)
{
    if (mSoundAsset)
        mSoundAsset->removeSourceAt(index);
}

bool Sound::removeAsset(Asset* asset)
{
    if (mSoundAsset)
        return mSoundAsset->removeSource(asset);
    return false;
}

QList<Asset*> Sound::assets() const
{
    QList<Asset*> assets;
    if (mSoundAsset)
        assets = mSoundAsset->sources();
    return assets;
}

QVariantMap Sound::toJsonObject(bool internal) const
{
    QVariantMap data = GameObject::toJsonObject(internal);
    data.insert("asset", mSoundAsset ? mSoundAsset->name() : "");
    return data;
}
