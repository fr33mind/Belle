#include "animationimage.h"

#include <QFileInfo>

#include "utils.h"
#include "resource_manager.h"

AnimatedImage::AnimatedImage(const QString& path) :
    ImageFile(path, false)
{
    //support for animated images
    mMovie = new QMovie(path);
     mMovie->jumpToFrame(0);
    QString movieName = mMovie->fileName();
    QString frameName = "";

    if (movieName.isEmpty())
        movieName = "image"; //shouldn't happen but just for precaution
    else //remove suffix
        movieName = QFileInfo(movieName).baseName();

    for(int i=0; i < mMovie->frameCount(); i++) {
        frameName = movieName + QString::number(i) + ".png"; //always save to PNG.
        mFramesNames.append(frameName);
        mMovie->jumpToNextFrame();
    }
}

AnimatedImage::~AnimatedImage()
{
    if (mMovie)
        mMovie->deleteLater();
}

void AnimatedImage::init()
{
}

bool AnimatedImage::isAnimated()
{
    if (mMovie && mMovie->isValid())
        return true;
    return false;
}

QMovie* AnimatedImage::movie()
{
    return mMovie;
}

QPixmap AnimatedImage::pixmap()
{
    if (mMovie)
        return mMovie->currentPixmap();
    return QPixmap();
}

int AnimatedImage::width()
{
    if (mMovie)
        return mMovie->currentPixmap().width();
    return 0;
}

int AnimatedImage::height()
{
    if (mMovie)
        return mMovie->currentPixmap().height();
    return 0;
}

bool AnimatedImage::isValid()
{
    if (mMovie)
        return mMovie->isValid();
    return false;
}

int AnimatedImage::frameNumber() const
{
    if (mMovie)
        return mMovie->currentFrameNumber();
    return -1;
}

QStringList AnimatedImage::framesNames() const
{
    return mFramesNames;
}

QRect AnimatedImage::rect() const
{
    if (mMovie)
        return mMovie->frameRect();

    return QRect();
}
