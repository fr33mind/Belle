#ifndef FONTASSET_H
#define FONTASSET_H

#include "multisourceasset.h"
#include "fontfile.h"

class FontFile;

class FontAsset : public MultiSourceAsset
{
public:
    explicit FontAsset();
    FontAsset(const QString&);
    FontAsset(const QVariantMap&);
    virtual ~FontAsset();

    int fontId() const;
    FontFile* mainFontFile() const;

    QString fontFamily() const;

    QString fontWeight() const;
    void setFontWeight(const QString&);

    QString fontStyle() const;
    void setFontStyle(const QString&);

    QString fallback() const;
    QStringList fallbacks() const;
    void setFallback(const QString&);

    QString genericFontFamily() const;
    void setGenericFontFamily(const QString&);

    virtual QVariantMap toJsonObject();
    QString toFontFace() const;

    static QStringList supportedFormats();

protected:
    virtual void sourceRemoved(Asset *);

private:
    QString mFontFamily;
    QString mFontWeight;
    QString mFontStyle;
    QString mFallback;
    QString mGenericFontFamily;
    int mFontId;

    void init();
    QString fontFaceUrls() const;
    void setFontFamily(const QString&);
    void setFontId(int);
};

#endif // FONTASSET_H
