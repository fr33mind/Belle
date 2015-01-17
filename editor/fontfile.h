#ifndef FONTFILE_H
#define FONTFILE_H

#include "asset.h"

class FontFile : public Asset
{
public:
    FontFile(const QString&);
    int id() const;

private:
    int mId;
};

#endif // FONTFILE_H
