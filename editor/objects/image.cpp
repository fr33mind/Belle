/* Copyright (C) 2012-2014 Carlos Pais
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "image.h"

#include <QDebug>
#include <QFileInfo>
#include <QMovie>

#include "imagetransform.h"

//Image::Image(QPixmap *image, QObject *parent, const QString& name) :
//    Object(parent, name)
//{
//    init();
//    setImage(new AnimatedImage(image));
//}

Image::Image(const QString& path, QObject *parent, const QString& name) :
    Object(parent, name)
{
    init();
    setImage(path);
    setName(QFileInfo(ResourceManager::mediaName(path)).baseName());
}

Image::Image(const QVariantMap& data, QObject* parent):
    Object(data, parent)
{
    init();

    if (data.contains("image")) {
        if (data.value("image").type() == QVariant::String)
            setImage(data.value("image").toString());
        else if (data.value("image").type() == QVariant::Map) {
            QVariantMap img = data.value("image").toMap();
            if (img.contains("src") && img.value("src").type() == QVariant::String)
                setImage(img.value("src").toString());
        }
    }
}

Image::~Image()
{
    ResourceManager::decrementReference(mImage);
}

void Image::init()
{
    setKeepAspectRatio(true);
    mImage = 0;
    setType("Image");
}

void Image::_setImage(ImageFile* image)
{
    if (image == mImage)
        return;

    if (mImage && mImage->isAnimated()) {
        mImage->movie()->stop();
        mImage->movie()->disconnect(this);
    }

    ResourceManager::decrementReference(mImage);
    mImage = image;

    if (mImage && mImage->isAnimated()) {
        connect(mImage->movie(), SIGNAL(frameChanged(int)), this, SIGNAL(dataChanged()));
        mImage->movie()->start();
    }

    if (mImage && mImage->isValid() && (width() == 0 || height() == 0)) {
        mSceneRect.setWidth(mImage->width());
        mSceneRect.setHeight(mImage->height());
    }

    updateAspectRatio();
    mImageTransform.setImage(mImage);
}

void Image::setImage(const QString& path)
{
    if (mImage && mImage->path() == path)
        return;

    _setImage(ResourceManager::newImage(path));
}

void Image::setImage(ImageFile* image)
{
    if (mImage == image)
        return;

    ResourceManager::incrementReference(image);
    _setImage(image);
}

ImageFile* Image::image() const
{
    return mImage;
}

void Image::paint(QPainter & painter)
{
    Object::paint(painter);

    if (mImage) {
        if (mImageTransform.hasToBeTransformed(mImage, sceneRect(), cornerRadius()))
            painter.drawPixmap(mSceneRect, mImageTransform.transform(mImage, sceneRect(), cornerRadius()));
        else
            painter.drawPixmap(mSceneRect, mImage->pixmap());
    }
}

void Image::show()
{
    if (mImage && mImage->isAnimated())
        mImage->movie()->start();
}

void Image::hide()
{
    if (mImage && mImage->isAnimated())
        mImage->movie()->stop();
}

QVariantMap Image::toJsonObject(bool internal)
{
    QVariantMap object = Object::toJsonObject(internal);

    if (mImage)
        object.insert("image", mImage->name());

    filterResourceData(object);
    return object;
}
