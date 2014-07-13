#ifndef IMAGETRANSFORM_H
#define IMAGETRANSFORM_H

#include <QHash>
#include <QPixmap>
#include <QVariant>

#include "imagefile.h"

class ImageTransform
{
public:
    enum TransformType {
        Stretch,
        Repeat,
        Center
    };

    explicit ImageTransform();

    bool hasToBeTransformed(ImageFile*, const QRect&, int) const;
    QPixmap transformedImage() const;
    QPixmap transform(ImageFile*, const QRect&, int);

    TransformType transformType() const;
    void setTransformType(TransformType type);

    ImageFile* image() const;
    void setImage(ImageFile*);

private:
    void updateCache(ImageFile* image, const QString&, int, int);
    bool isCached(const QRect&, int) const;

private:
    ImageFile* mImage;
    QPixmap mTransformedImage;
    TransformType mTransformType;
    QHash<QString, QVariant> mCache;
};

#endif // IMAGETRANSFORM_H
