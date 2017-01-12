#ifndef FONT_H
#define FONT_H

#include "gameobject.h"
#include "fontasset.h"

class FontAsset;

class Font : public GameObject
{
    Q_OBJECT
public:
    explicit Font(QObject *parent = 0);
    Font(const QString&, QObject *parent = 0);
    Font(const QVariantMap&, QObject *parent = 0);
    virtual ~Font();

    Asset* setFile(const QString&);
    Asset* addFile(const QString&);

    FontAsset* asset() const;

    virtual QVariantMap toJsonObject(bool internal=true) const;

protected:
    virtual void loadData(const QVariantMap&, bool internal=false);

signals:

public slots:

private:
    void init();
    FontAsset* mFontAsset;
};

#endif // FONT_H
