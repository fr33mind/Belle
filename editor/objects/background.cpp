#include "background.h"

#include <QPainter>
#include <QDebug>

#include "resource_manager.h"

Background::Background()
{
    mImage = 0;
    mColor = QColor();
    mOpacity = 0;
    mReleaseImage = false;
}

Background::~Background()
{
    releaseImage();
}

qreal Background::opacityF() const
{
    return mOpacity / 255.0;
}

int Background::opacity() const
{
    return mOpacity;
}

void Background::setOpacity(int a)
{
    mOpacity = a;
    mColor.setAlpha(a);
}

QColor Background::color() const
{
    return mColor;
}

void Background::setColor(const QColor& color)
{
    mColor = color;
    mColor.setAlpha(mOpacity);
}

ImageTransform::TransformType Background::positioning() const
{
    return mImageTransform.transformType();
}

void Background::setPositioning(ImageTransform::TransformType p)
{
    mImageTransform.setTransformType(p);
}

ImageFile* Background::image() const
{
    return mImage;
}

void Background::setImage(const QString& path)
{
    ImageFile* img = dynamic_cast<ImageFile*>(AssetManager::instance()->loadAsset(path, Asset::Image));
    if (mImage == img) {
        AssetManager::instance()->releaseAsset(img);
        return;
    }

    setImage(img);
    mReleaseImage = true;
}

void Background::setImage(ImageFile* image)
{
    if (mImage == image)
        return;

    releaseImage();
    mImage = image;
}

QString Background::path() const
{
    if (mImage)
        return mImage->path();
    return "";
}

void Background::paint(QPainter &p, const QRect &rect, int radius, float opacity)
{
    p.save();

    if (mImage || mColor.isValid())
        p.setOpacity(opacityF()*opacity);
    else
        p.setOpacity(0.0);

    p.setPen(Qt::NoPen); //don't add border
    if (mColor.isValid())
        p.setBrush(mColor);

    if (radius)
        p.drawRoundedRect(rect, radius, radius);
    else
        p.drawRect(rect);

    if (mImage) {
        QPixmap pixmap = mImage->pixmap();
        if (mImageTransform.hasToBeTransformed(mImage, rect, radius))
            p.drawPixmap(rect, mImageTransform.transform(mImage, rect, radius));
        else if (! pixmap.isNull())  {
            p.drawPixmap(rect, pixmap); //for now just draw stretched background
        }
    }

    p.restore();
}

bool Background::isValid() const
{
    if (mImage || mColor.isValid())
        return true;
    return false;
}

void Background::releaseImage()
{
    if (mImage && mReleaseImage)
        AssetManager::instance()->releaseAsset(mImage);

    mImage = 0;
    mReleaseImage = false;
}
