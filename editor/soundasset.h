#ifndef SOUNDASSET_H
#define SOUNDASSET_H

#include "multisourceasset.h"

class SoundAsset : public MultiSourceAsset
{
public:
    SoundAsset(const QString& path);
    SoundAsset(const QVariantMap& data);
    virtual ~SoundAsset() {}

    static QStringList supportedFormats();
};

#endif // SOUNDASSET_H
