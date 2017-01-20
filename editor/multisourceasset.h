#ifndef MULTISOURCEASSET_H
#define MULTISOURCEASSET_H

#include "asset.h"

class MultiSourceAsset : public Asset
{

public:
    enum Type {
        Audio=Asset::Audio,
        Video=Asset::Video,
        Font=Asset::Font
    };

    MultiSourceAsset(MultiSourceAsset::Type);
    MultiSourceAsset(const QString& path, MultiSourceAsset::Type);
    MultiSourceAsset(const QVariantMap& data, MultiSourceAsset::Type);
    virtual ~MultiSourceAsset();
    virtual bool isValid() const;
    virtual bool isNull() const;
    virtual bool save(const QDir&, bool updatePath=false);
    virtual bool remove();
    Asset* addSource(const QString&);
    void addSource(Asset*);
    Asset* sourceAt(int) const;
    void removeSourceAt(int);
    bool removeSource(Asset*);
    QList<Asset*> sources() const;
    bool containsSource(const QString&);
    Asset* source(const QString&);
    virtual QVariantMap toJsonObject();

protected:
    virtual bool doSave(const QDir&);
    virtual void sourceRemoved(Asset*);

private:
    QList<Asset*> mSources;
    static Asset::Type sourceType(Asset::Type);
    void init();
};

#endif // MULTISOURCEASSET_H
