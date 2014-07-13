#ifndef AnimatedImage_H
#define AnimatedImage_H

#include <QObject>
#include <QMovie>
#include <QPixmap>
#include <QDir>

#include "imagefile.h"

class ImageFile;

class AnimatedImage : public ImageFile
{
    QMovie* mMovie;
    QStringList mFramesNames;

public:
    explicit AnimatedImage(const QString& path="");
    AnimatedImage(QPixmap*);
    virtual ~AnimatedImage();

    QPixmap pixmap();
    bool isAnimated();
    bool isValid();
    QMovie* movie();
    int width();
    int height();
    int frameNumber() const;
    QStringList framesNames() const;
    QRect rect() const;
    
signals:
    
public slots:
    
private:
   void init();
};

#endif // AnimatedImage_H
