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

    QPixmap pixmap() const;
    bool isAnimated() const;
    bool isValid() const;
    QMovie* movie() const;
    int width() const;
    int height() const;
    int frameNumber() const;
    QStringList framesNames() const;
    QRect rect() const;
    virtual QVariantMap toJsonObject();
    
protected:
    virtual void checkTransparency();

signals:
    
public slots:
    
private:
   void init();
};

#endif // AnimatedImage_H
