#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <QPixmap>
#include <QImage>
#include <QColor>

#include "animatedimage.h"
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
    qreal opacityF() const;
    void setOpacity(int);

    QColor color() const;
    void setColor(const QColor&);

    ImageTransform::TransformType positioning() const;
    void setPositioning(ImageTransform::TransformType);

    bool isValid() const;

private:
    void releaseImage();

    ImageFile *mImage;
    ImageTransform mImageTransform;
    QColor mColor;
    int mOpacity;
    bool mReleaseImage;
};

#endif // BACKGROUND_H
