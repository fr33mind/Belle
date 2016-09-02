#include "imagefile.h"

#include <QMovie>
#include <QFileInfo>

#include "animatedimage.h"

ImageFile::ImageFile(const QString&path, bool load) :
    Asset(path, Asset::Image)
{
    if (load)
        mPixmap = QPixmap(path);
    mPath = path;
    //no need to check for transparency for non-animated images
    mTransparent = false;
}

ImageFile::~ImageFile()
{
}

bool ImageFile::isAnimated() const
{
    return false;
}

QMovie* ImageFile::movie() const
{
    return 0;
}

int ImageFile::frameNumber() const
{
    if (! mPixmap.isNull())
        return 0;
    return -1;
}

QPixmap ImageFile::pixmap() const
{
    return mPixmap;
}

bool ImageFile::isNull() const
{
    return mPixmap.isNull();
}

int ImageFile::width() const
{
    return mPixmap.width();
}

int ImageFile::height() const
{
    return mPixmap.height();
}

QRect ImageFile::rect() const
{
    return mPixmap.rect();
}

void ImageFile::checkTransparency()
{
    mTransparent = ImageFile::isTransparent(mPixmap.toImage());
}

bool ImageFile::isTransparent() const
{
    return mTransparent;
}

bool ImageFile::isAnimated(const QString& path)
{
    bool animated = false;
    QImageReader reader(path);
    if (reader.canRead() && reader.supportsAnimation() && reader.imageCount() > 1) {
        animated = true;
    }

    return animated;
}

ImageFile* ImageFile::create(const QString& path)
{
    if (ImageFile::isAnimated(path))
        return new AnimatedImage(path);

    return new ImageFile(path);
}

bool ImageFile::isTransparent(const QImage& image)
{
    bool useAlpha = false;
    const uchar* pixelData = image.bits();
    int bytes = image.byteCount();

    for (const QRgb* pixel = reinterpret_cast<const QRgb*>(pixelData); bytes > 0; pixel++, bytes -= sizeof(QRgb)) {
        if (qAlpha(*pixel) != UCHAR_MAX) {
            useAlpha = true;
            break;
        }
    }

    return useAlpha;
}

QStringList ImageFile::supportedFormats()
{
    QStringList formats;
    formats << "png" << "xpm" << "jpg" << "jpeg" << "gif" << "svg" << "svgz" << "bmp";
    return formats;
}
