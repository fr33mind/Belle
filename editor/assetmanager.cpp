#include "assetmanager.h"

#include <QFile>
#include <QFontDatabase>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QScopedPointer>

#include "utils.h"
#include "fontfile.h"
#include "soundasset.h"
#include "fontasset.h"

static AssetManager* mInstance = new AssetManager();
static QScopedPointer<AssetManager> mInstanceScopedPointer(mInstance);

AssetManager::AssetManager()
{
    mTypeToPath.insert(Asset::Image, "");
    mTypeToPath.insert(Asset::Audio, "");
    mTypeToPath.insert(Asset::Font, "");
    mTypeToPath.insert(Asset::Unknown, "");

    mImageFormats << "bmp" << "gif" << "ico" << "jpeg" << "jpg" << "mng" << "png" << "svg" << "svgz" << "tif" << "tiff" << "webp" << "xbm" << "xpm";
    mAudioFormats << "mp3" << "aac" << "ogg" << "oga" << "webm" << "wav" << "m4a" << "mp4";
    mVideoFormats << "mp4" << "m4v" << "webm" << "ogv" << "ogg";
    mFontFormats << "ttf" << "otf" << "eot" << "woff";
}

AssetManager::~AssetManager()
{
    clear();
}

AssetManager* AssetManager::instance()
{
    return mInstance;
}

void AssetManager::setLoadPath(const QString & path)
{
    mLoadPath = path;
}

QString AssetManager::absoluteFilePath(const QString & name, Asset::Type type)
{
    if (QFileInfo(name).isAbsolute())
        return name;

    if (! mLoadPath.isEmpty()) {
         QDir dir(mLoadPath);
         if (! mTypeToPath[type].isEmpty() && dir.exists(mTypeToPath[type]))
             dir.cd(mTypeToPath[type]);
         if (dir.exists(name))
             return dir.absoluteFilePath(name);
    }

    return name;
}

Asset* AssetManager::asset(const QString & path, Asset::Type type) const
{
    if (type == Asset::Unknown)
        type = guessType(path);

    bool absolute = QFileInfo(path).isAbsolute();
    QList<Asset*> assets = mAssets.keys();
    for(int i=0; i < assets.size(); i++) {
        if (assets[i]->type() != type)
            continue;

        if (absolute && assets[i]->path() == path || (! absolute) && assets[i]->name() == path)
            return assets[i];
    }
    return 0;
}

QList<Asset*> AssetManager::assets() const
{
    return mAssets.keys();
}

QList<Asset*> AssetManager::assets(Asset::Type type) const
{
    QList<Asset*> assets = mAssets.keys();
    for(int i=assets.size()-1; i >= 0; i--)
        if (assets[i]->type() != type)
            assets.removeAt(i);
    return assets;
}

Asset* AssetManager::loadAsset(QString path, Asset::Type type)
{
    if (path.isEmpty())
        return 0;

    path = absoluteFilePath(path);
    Asset* asset = this->asset(path, type);
    if (asset) {
        int count = mAssets.value(asset, 0);
        mAssets.insert(asset, ++count);
        return asset;
    }

    asset = _loadAsset(path, type);
    if (asset && mFilesToRemove.contains(asset->path())) {
        mFilesToRemove.remove(asset->path());
        asset->setRemovable(true);
    }

    return asset;
}

Asset* AssetManager::_loadAsset(const QVariantMap& data, Asset::Type type)
{
    Asset* asset = 0;

    if (type == Asset::Audio) {
        asset = new SoundAsset(data);
        addAsset(asset, type);
    }
    else if (type == Asset::Font) {
        asset = new FontAsset(data);
        addAsset(asset, type);
    }
    else {
        asset = _loadAsset(data.value("name").toString(), type);
    }

    return asset;
}

Asset* AssetManager::_loadAsset(const QString& name, Asset::Type type)
{
    QString path = absoluteFilePath(name);

    if (! QFile::exists(path))
        return 0;

    Asset* asset = 0;
    if (type == Asset::Image)
        asset = ImageFile::create(path);
    else if (type == Asset::Font)
        asset = new FontAsset(path);
    else if (type == Asset::Audio)
        asset = new SoundAsset(path);
    else if (type == Asset::FontSource)
        asset = new FontFile(path);
    else
        asset = new Asset(path, type);

    addAsset(asset, type);
    return asset;
}

void AssetManager::releaseAsset(Asset* asset)
{
    if (! asset)
        return;

    int count = mAssets.value(asset, 0);
    if (count) {
        --count;
        if (count <= 0) {
            removeAsset(asset);
        }
        else {
            mAssets.insert(asset, count);
        }
    }
}

void AssetManager::removeAsset(Asset* asset)
{
    if (!asset || !mAssets.contains(asset))
        return;

    mAssets.remove(asset);
    if (asset->isRemovable())
        mFilesToRemove.insert(asset->path());
    delete asset;
}

void AssetManager::clearAssets()
{
    QList<Asset*> assets = mAssets.keys();
    for(int i=0; i < assets.size(); i++) {
        removeAsset(assets.at(i));
    }
    mAssets.clear();
    mFilesToRemove.clear();
}

ImageFile* AssetManager::image(const QString & name)
{
   return dynamic_cast<ImageFile*>(asset(name));
}

bool AssetManager::isNameUnique(const QString& name, const QString& destPath) const
{
    if (name.isEmpty())
        return false;

    QList<Asset*> assets = mAssets.keys();
    for(int i=0; i < assets.size(); i++)
        if (assets[i]->name() == name && mTypeToPath.value(assets[i]->type(), "") == destPath)
            return false;

    return true;
}


QString AssetManager::uniqueName(QString name, const QString& destPath) const
{
    if (name.isEmpty() || name.isNull())
        name = "asset";

    while(! isNameUnique(name, destPath))
        name = Utils::incrementFileName(name);

    return name;
}

Asset::Type AssetManager::guessType(const QString & name) const
{
    QFileInfo info(name);
    QString ext = info.suffix();
    if (mImageFormats.contains(ext, Qt::CaseInsensitive))
        return Asset::Image;
    if (mAudioFormats.contains(ext, Qt::CaseInsensitive))
        return Asset::Audio;
    if (mVideoFormats.contains(ext, Qt::CaseInsensitive))
        return Asset::Video;
    if (mFontFormats.contains(ext, Qt::CaseInsensitive))
        return Asset::Font;
    return Asset::Unknown;
}

QVariantMap AssetManager::readAssetsFile(const QString& filepath)
{
    QFile file(filepath);
    if (! file.open(QFile::ReadOnly | QFile::Text))
        return QVariantMap();

    QByteArray contents = file.readAll();
    //remove start "game.assets ="
    int i = 0;
    for(i=0; i < contents.size() && contents[i] != '{'; i++);
    contents = contents.mid(i);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(contents, &error);
    if (error.error != QJsonParseError::NoError)
        return QVariantMap();

    QVariant data = doc.toVariant();
    if (data.type() != QVariant::Map)
        return QVariantMap();

    return data.toMap();
}

void AssetManager::load(const QDir & dir, bool fromProject)
{
    QVariantMap data = readAssetsFile(dir.absoluteFilePath(ASSETS_FILE));
    if (data.isEmpty())
        return;

    QVariantMap subdirs = data.value("subdirs").toMap();
    mTypeToPath[Asset::Image] = subdirs.value("images", "").toString();
    mTypeToPath[Asset::Audio] = subdirs.value("sounds", "").toString();
    mTypeToPath[Asset::Font] = subdirs.value("fonts", "").toString();

    QVariantMap item;
    Asset* asset = 0;

    QVariantList imagesData = data.value("images").toList();
    for(int i=0; i < imagesData.size(); i++) {
        item = imagesData[i].toMap();
        asset = _loadAsset(item.value("name", "").toString(), Asset::Image);
        if (asset && fromProject)
            asset->setRemovable(true);
    }

    QVariantList audioData = data.value("sounds").toList();
    for(int i=0; i < audioData.size(); i++) {
        item = audioData[i].toMap();
        if (item.contains("sources")) {
            asset = _loadAsset(item, Asset::Audio);
        }
        else {
            //for backwards compatibility
            asset = _loadAsset(item.value("name").toString(), Asset::Audio);
        }
        if (asset && fromProject)
            asset->setRemovable(true);
    }

    QVariantList fontsData = data.value("fonts").toList();
    for(int i=0; i < fontsData.size(); i++) {
        item = fontsData[i].toMap();
        if (item.contains("sources")) {
            asset = _loadAsset(item, Asset::Font);
        }
        else {
            //for backwards compatibility
            asset = _loadAsset(item.value("name", "").toString(), Asset::Font);
        }
        if (asset && fromProject)
            asset->setRemovable(true);
    }

    //reset reference counts
    QList<Asset*> assets = mAssets.keys();
    for(int i=0; i < assets.size(); i++)
        mAssets[assets[i]] = 0;
}

void AssetManager::save(const QDir & dir, bool toProject)
{
    QVariantMap data;
    QFile file(dir.absoluteFilePath(ASSETS_FILE));
    if (! file.open(QFile::WriteOnly | QFile::Text))
        return;

    QVariantMap subdirs;
    subdirs.insert("images", mTypeToPath.value(Asset::Image));
    subdirs.insert("sounds", mTypeToPath.value(Asset::Audio));
    subdirs.insert("fonts", mTypeToPath.value(Asset::Font));
    data.insert("subdirs", subdirs);

    QList<Asset*> images = this->assets(Asset::Image);
    QVariantList imagesData;
    for(int i=0; i < images.size(); i++) {
        imagesData.append(images[i]->toJsonObject());
        bool saved = images[i]->save(dir, toProject);
        if (saved && toProject)
            images[i]->setRemovable(true);
    }

    QList<Asset*> sounds = this->assets(Asset::Audio);
    QVariantList soundsData;
    for(int i=0; i < sounds.size(); i++) {
        soundsData.append(sounds[i]->toJsonObject());
        sounds[i]->save(dir, toProject);
        if (toProject)
            sounds[i]->setRemovable(true);
    }

    QList<Asset*> fonts = this->assets(Asset::Font);
    QVariantList fontsData;
    for(int i=0; i < fonts.size(); i++) {
        fontsData.append(fonts[i]->toJsonObject());
        fonts[i]->save(dir, toProject);
        if (toProject)
            fonts[i]->setRemovable(true);
    }

    data.insert("images", imagesData);
    data.insert("sounds", soundsData);
    data.insert("fonts", fontsData);

    file.write("game.assets = ");
    file.write(QJsonDocument::fromVariant(data).toJson(QJsonDocument::Compact));
    file.close();

    saveFontFaces(fonts, dir);

    if (toProject)
        cleanup();
}

void AssetManager::saveFontFaces(const QList<Asset*>& fonts, const QDir& dir)
{
    if (fonts.isEmpty())
        return;

    QString fontfaces(FONTFACES_FILE);
    QFile file(dir.absoluteFilePath(fontfaces));
    if (! file.open(QFile::WriteOnly | QFile::Text))
        return;

    FontAsset* font = 0;
    QString fontface;

    for(int i=0; i < fonts.size(); i++) {
        font = dynamic_cast<FontAsset*>(fonts[i]);
        if (! font) continue;
        //TODO: Add support for relative paths in font-face declaration
        fontface = font->toFontFace();
        file.write(fontface.toUtf8() + "\n");
    }

    file.close();
}

QList<int> AssetManager::fontsIds()
{
    QList<int> ids;
    QList<Asset*> fontfiles = this->assets(Asset::Font);
    for(int i=0; i < fontfiles.size(); i++) {
        FontAsset* font = static_cast<FontAsset*>(fontfiles[i]);
        ids.append(font->fontId());
    }

    return ids;
}

void AssetManager::cleanup()
{
    foreach(const QString& path, mFilesToRemove) {
        if (QFile::exists(path))
            QFile::remove(path);
    }

    mFilesToRemove.clear();
}

void AssetManager::clear()
{
    setLoadPath("");
    clearAssets();
}

void AssetManager::addAsset(Asset * asset, Asset::Type type)
{
    QString destPath = mTypeToPath.value(type, "");
    if (asset && ! isNameUnique(asset->name(), destPath))
        asset->setName(uniqueName(asset->name(), destPath));

    mAssets.insert(asset, 1);
}
