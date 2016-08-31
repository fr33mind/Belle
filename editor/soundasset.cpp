#include "soundasset.h"

SoundAsset::SoundAsset(const QString& path) :
    MultiSourceAsset(path, MultiSourceAsset::Audio)
{
}

SoundAsset::SoundAsset(const QVariantMap& data) :
    MultiSourceAsset(data, MultiSourceAsset::Audio)
{
}

QStringList SoundAsset::supportedFormats()
{
    QStringList formats;
    formats << "ogg" << "oga" << "mp3" << "mp4" << "m4a" << "aac" << "wav" << "wave" << "webm";
    return formats;
}
