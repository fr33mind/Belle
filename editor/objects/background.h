#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <QPixmap>
#include <QImage>
#include <QColor>

#include "animationimage.h"
#include "imagetransform.h"

class Background
{

public:

    Background();
    ~Background();

    void paint(QPainter&, const QRect&, int radius=0, float opacity=1);

    QString path() const;

    ImageFile* image() const;
    void setImage(const QString&);
    void setImage(ImageFile*);

    int opacity() const;
    void setOpacity(int);

    QColor color() const;
    void setColor(const QColor&);

    ImageTransform::TransformType positioning() const;
    void setPositioning(ImageTransform::TransformType);

private:
    ImageFile *mImage;
    ImageTransform mImageTransform;
    QColor mColor;
};

#endif // BACKGROUND_H
