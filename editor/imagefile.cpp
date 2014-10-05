#include "imagefile.h"

#include <QMovie>
#include <QFileInfo>

#include "animationimage.h"

ImageFile::ImageFile()
{
}

ImageFile::ImageFile(const QString&path, bool load)
{
    if (load)
        mPixmap = QPixmap(path);
    mPath = path;
}

ImageFile::~ImageFile()
{
}

bool ImageFile::isAnimated()
{
    return false;
}

QMovie* ImageFile::movie()
{
    return 0;
}

int ImageFile::frameNumber() const
{
    if (! mPixmap.isNull())
        return 0;
    return -1;
}

QPixmap ImageFile::pixmap()
{
    return mPixmap;
}

QString ImageFile::path() const
{
    return mPath;
}

QString ImageFile::fileName() const
{
    return QFileInfo(mPath).fileName();
}

QString ImageFile::name() const
{
    return mName;
}

void ImageFile::setName(const QString& name)
{
    mName = name;
}

bool ImageFile::isValid()
{
    return !mPixmap.isNull();
}

int ImageFile::width()
{
    return mPixmap.width();
}

int ImageFile::height()
{
    return mPixmap.height();
}

QRect ImageFile::rect()
{
    return mPixmap.rect();
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

bool ImageFile::save(const QString & path)
{
    //save image in memory to path
    if (! mPixmap.isNull())
        return mPixmap.save(path);
    return false;
}


