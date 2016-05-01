#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <QDir>
#include <QVariantMap>
#include <QString>
#include <QSet>

#include "asset.h"
#include "imagefile.h"

#define FONTFACES_FILE "fontfaces.css"
#define ASSETS_FILE "assets.js"

class AssetManager
{
    QHash<Asset*, int> mAssets;
    QHash<Asset::Type, QString> mTypeToPath;
    QSet<QString> mFilesToRemove;
    QString mLoadPath;
    QStringList mImageFormats;
    QStringList mAudioFormats;
    QStringList mVideoFormats;
    QStringList mFontFormats;

public:
    AssetManager();
    static AssetManager* instance();
    void load(const QDir&, bool fromProject=false);
    void save(const QDir&, bool toProject=false);
    Asset* asset(const QString&, Asset::Type type=Asset::Unknown) const;
    QList<Asset*> assets() const;
    QList<Asset*> assets(Asset::Type type) const;
    Asset* loadAsset(QString, Asset::Type=Asset::Unknown);
    void releaseAsset(Asset*);
    void removeAsset(Asset*);
    void clearAssets();
    ImageFile* image(const QString&);
    QList<int> fontsIds();
    void clear();

    bool isNameUnique(const QString&, const QString&destPath="") const;
    QString uniqueName(QString, const QString&destPath="") const;
    QString absoluteFilePath(const QString&, Asset::Type=Asset::Unknown);
    void setLoadPath(const QString&);

protected:
    QVariantMap readAssetsFile(const QString&);
    void saveFontFaces(const QList<Asset*>&, const QDir&);
    void updateRefCount();
    Asset* _loadAsset(const QString&, Asset::Type) const;
    Asset::Type guessType(const QString&) const;

private:
    void cleanup();

};

#endif // ASSETMANAGER_H
