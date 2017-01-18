#ifndef FONTFILE_H
#define FONTFILE_H

#include "asset.h"

class FontFile : public Asset
{
public:
    FontFile(const QString&);
    virtual ~FontFile();
    int id() const;
    virtual bool isNull() const;
    QStringList fontFamilies() const;
    QString fontFamily() const;

private:
    int mId;
};

#endif // FONTFILE_H
