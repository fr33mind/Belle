#include "background.h"

#include <QPainter>
#include <QDebug>

#include "resource_manager.h"

Background::Background()
{
    mImage = 0;
    mColor = QColor();
}

Background::~Background()
{
    if (mImage)
        ResourceManager::decrementReference(mImage);
}

int Background::opacity() const
{
    return mColor.alpha();
}

void Background::setOpacity(int a)
{
    mColor.setAlpha(a);
}

QColor Background::color() const
{
    return mColor;
}

void Background::setColor(const QColor& color)
{
    mColor = color;
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
    if (mImage && mImage->path() == path)
        return;

    mImage = ResourceManager::newImage(path);
}

void Background::setImage(ImageFile* image)
{
    if (mImage == image)
        return;
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

    p.setOpacity(mColor.alphaF()*opacity);
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
