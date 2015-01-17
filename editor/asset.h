#ifndef ASSET_H
#define ASSET_H

#include <QString>
#include <QVariant>
#include <QDir>

class Asset
{

public:
    enum Type {
        Image,
        Audio,
        Video,
        Font,
        Unknown
    };

public:
    Asset(const QString&, Type);
    virtual ~Asset(){}
    void setName(const QString&);
    QString name() const;
    QString path() const;
    Type type() const;
    virtual bool isValid() const;
    virtual void save(const QDir&);
    virtual QVariantMap toJsonObject();

private:
    void init();

    QString mPath;
    QString mName;
    Type mType;
};

#endif // ASSET_H
