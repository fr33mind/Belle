#include "animatedimage.h"

#include <QFileInfo>
#include <QByteArray>
#include <QBuffer>
#include <QImage>

#include "utils.h"
#include "resource_manager.h"

AnimatedImage::AnimatedImage(const QString& path) :
    ImageFile(path, false)
{
    //support for animated images
    mMovie = new QMovie(path);
    mMovie->jumpToFrame(0);
    checkTransparency();
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

bool AnimatedImage::isAnimated() const
{
    if (mMovie && mMovie->isValid())
        return true;
    return false;
}

QMovie* AnimatedImage::movie() const
{
    return mMovie;
}

QPixmap AnimatedImage::pixmap() const
{
    if (mMovie)
        return mMovie->currentPixmap();
    return QPixmap();
}

int AnimatedImage::width() const
{
    if (mMovie)
        return mMovie->currentPixmap().width();
    return 0;
}

int AnimatedImage::height() const
{
    if (mMovie)
        return mMovie->currentPixmap().height();
    return 0;
}

bool AnimatedImage::isValid() const
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

void AnimatedImage::checkTransparency()
{
    mTransparent = false;
    if (! mMovie || ! mMovie->isValid())
        return;

    mMovie->jumpToFrame(0);
    bool transparent = false;
    for(int i=0; i < mMovie->frameCount(); i++) {
        bool res =  mMovie->jumpToNextFrame();
        if (! res)
            continue;
        transparent = ImageFile::isTransparent(mMovie->currentImage());
        if (transparent)
            break;
    }

    mTransparent = transparent;
}

QVariantMap AnimatedImage::toJsonObject()
{
    QVariantMap data = Asset::toJsonObject();
    if (!mMovie || ! mMovie->isValid())
        return data;

    int currFrame = -1;
    if (mMovie->state() == QMovie::Running) {
        currFrame = mMovie->currentFrameNumber();
        mMovie->stop();
    }

    QString format("JPG");
    if (isTransparent())
        format = "PNG";

    QVariantList frames;
    mMovie->jumpToFrame(0);
    for(int i=0; i < mMovie->frameCount(); i++) {
        bool res =  mMovie->jumpToNextFrame();
        if (! res)
            continue;
        QImage image = mMovie->currentImage();
        QByteArray arr;
        QBuffer buf(&arr);
        buf.open(QIODevice::WriteOnly);
        image.save(&buf, format.toLatin1());
        buf.close();
        QByteArray imageData;
        imageData.append(QString("data:image/%1;base64,").arg(format.toLower()));
        imageData.append(arr.toBase64());
        QVariantMap frameData;
        frameData.insert("data", imageData);
        frameData.insert("delay", mMovie->nextFrameDelay());
        frames.append(frameData);
    }

    if (currFrame != -1) {
        mMovie->jumpToFrame(currFrame);
        mMovie->start();
    }

    data.insert("frames", frames);
    return data;
}
