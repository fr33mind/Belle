#ifndef IMAGEFILE_H
#define IMAGEFILE_H

#include <QString>
#include <QPixmap>
#include <QMovie>

class ImageFile
{
public:
    explicit ImageFile();
    ImageFile(const QString&, bool load=true);
    virtual ~ImageFile();

    virtual bool isAnimated();
    virtual QMovie* movie();
    virtual int frameNumber() const;
    virtual QPixmap pixmap();
    virtual int width();
    virtual int height();
    virtual QRect rect();
    virtual bool isValid();
    QString path() const;
    QString fileName() const;
    QString name() const;
    void setName(const QString&);
    bool save(const QString&);

    static bool isAnimated(const QString&);
    static ImageFile* create(const QString&);

private:
    QPixmap mPixmap;
    QString mPath;
    QString mName;
};

#endif // IMAGEFILE_H
