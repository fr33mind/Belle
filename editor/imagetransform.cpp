#include "imagetransform.h"

#include <QPainter>

ImageTransform::ImageTransform()
{
    clearCache();
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

void ImageTransform::updateCache(ImageFile* image, int frameNumber, int cornerRadius)
{
    mImage = image;
    mFrameNumber = frameNumber;
    mCornerRadius = cornerRadius;
}

bool ImageTransform::isCached(ImageFile* image, const QRect & rect, int radius) const
{
    if (!mImage || mImage != image)
        return false;

    if (! mTransformedImage.isNull() && mTransformedImage.width() == rect.width() && mTransformedImage.height() == rect.height() &&
            mCornerRadius == radius && image && mFrameNumber == image->frameNumber())
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
    if (!image)
        return QPixmap();

    if (! hasToBeTransformed(image, rect, radius))
        return image->pixmap();

    if (isCached(image, rect, radius))
        return mTransformedImage;

    QImage out(rect.width(), rect.height(), QImage::Format_ARGB32_Premultiplied);
    out.fill(0);
    QPainter p(&out);
    QPixmap pixmap = image->pixmap();

    //add support for other positions in the future
    if (mTransformType == Stretch)
        pixmap = pixmap.scaled(rect.width(), rect.height());
    p.setPen(Qt::NoPen);
    p.setBrush(QBrush(pixmap));
    p.drawRoundedRect(out.rect(), radius, radius);
    p.end();
    mTransformedImage = QPixmap::fromImage(out);

    updateCache(image, image->frameNumber(), radius);

    return mTransformedImage;
}

void ImageTransform::clearCache()
{
    mImage = 0;
    mFrameNumber = 0;
    mCornerRadius = 0;
}
