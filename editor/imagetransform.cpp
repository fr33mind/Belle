#include "imagetransform.h"

#include <QPainter>

ImageTransform::ImageTransform()
{
    mImage = 0;
    mTransformType = Stretch;
}

ImageFile* ImageTransform::image() const
{
    return mImage;
}

void ImageTransform::setImage(ImageFile* image)
{
    mImage = image;
}

ImageTransform::TransformType ImageTransform::transformType() const
{
    return mTransformType;
}

void ImageTransform::setTransformType(TransformType type)
{
    mTransformType = type;
}

QPixmap ImageTransform::transformedImage() const
{
    return mTransformedImage;
}

void ImageTransform::updateCache(ImageFile* image, const QString& path, int frameNumber, int cornerRadius)
{
    mImage = image;
    mCache.insert("path", path);
    mCache.insert("frameNumber", frameNumber);
    mCache.insert("cornerRadius", cornerRadius);
}

bool ImageTransform::isCached(const QRect & rect, int radius) const
{
    if (! mTransformedImage.isNull() && mTransformedImage.width() == rect.width() && mTransformedImage.height() == rect.height() &&
            mCache["cornerRadius"].toInt() == radius && mCache["path"].toString() == mImage->path() && mCache["frameNumber"].toInt() == mImage->frameNumber())
        return true;
    return false;
}

bool ImageTransform::hasToBeTransformed(ImageFile* image, const QRect & rect, int radius) const
{
    if (! image)
        return false;

    if (radius > 0)
        return true;

    if (mTransformType == Repeat && (image->width() < rect.width() || image->height() < rect.height()))
        return true;

    return false;
}

QPixmap ImageTransform::transform(ImageFile* image, const QRect & rect, int radius)
{
    if (! hasToBeTransformed(image, rect, radius))
        return QPixmap();

    if (isCached(rect, radius))
        return mTransformedImage;

    QImage out(rect.width(), rect.height(), QImage::Format_ARGB32_Premultiplied);
    out.fill(0);
    QPainter p(&out);
    QPixmap pixmap = image->pixmap();

    //add support for other positions in the future
    if (mTransformType == Stretch)
        pixmap = pixmap.scaled(rect.width(), rect.height());
    p.setBrush(QBrush(pixmap));
    p.drawRoundedRect(out.rect(), radius, radius);
    p.end();
    mTransformedImage = QPixmap::fromImage(out);

    updateCache(image, image->path(), image->frameNumber(), radius);

    return mTransformedImage;
}
