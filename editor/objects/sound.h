#ifndef SOUND_H
#define SOUND_H

#include "gameobject.h"
#include "soundasset.h"

class SoundAsset;

class Sound : public GameObject
{
    Q_OBJECT
public:
    explicit Sound(QObject *parent = 0);
    Sound(const QString&, QObject *parent = 0);
    Sound(const QVariantMap&, QObject *parent = 0);
    virtual ~Sound();
    Asset* setFile(const QString&);
    Asset* addFile(const QString&);
    Asset* assetAt(int) const;
    void removeAssetAt(int);
    bool removeAsset(Asset*);
    QList<Asset*> assets() const;
    virtual QVariantMap toJsonObject(bool internal=true) const;

signals:

protected:
    virtual void loadData(const QVariantMap&, bool internal=false);

private:
    void init();
    SoundAsset* mSoundAsset;

};

#endif // SOUND_H
