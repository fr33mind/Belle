#ifndef IMAGEFILE_H
#define IMAGEFILE_H

#include <QString>
#include <QPixmap>
#include <QMovie>

#include "asset.h"

class ImageFile : public Asset
{
public:
    ImageFile(const QString&, bool load=true);
    virtual ~ImageFile();

    virtual bool isAnimated() const;
    virtual QMovie* movie() const;
    virtual int frameNumber() const;
    virtual QPixmap pixmap() const;
    virtual int width() const;
    virtual int height() const;
    virtual QRect rect() const;
    virtual bool isNull() const;

    static bool isAnimated(const QString&);
    static ImageFile* create(const QString&);
    static bool isTransparent(const QImage&);

protected:
    bool mTransparent;
    bool isTransparent() const;
    virtual void checkTransparency();

private:
    QPixmap mPixmap;
    QString mPath;
    QString mName;

};

#endif // IMAGEFILE_H
