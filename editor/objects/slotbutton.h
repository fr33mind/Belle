#ifndef SLOTBUTTON_H
#define SLOTBUTTON_H

#include "objectgroup.h"
#include "image.h"

class Image;

class SlotButton : public ObjectGroup
{
    Q_OBJECT
public:
    enum SlotType {
        Save=0,
        Load
    };

    explicit SlotButton(QObject *parent = 0);
    SlotButton(const QVariantMap&, QObject *parent = 0);
    virtual ~SlotButton();

    int slot() const;
    void setSlot(int);

    Image* thumbnail() const;

    bool thumbnailEnabled() const;
    void setThumbnailEnabled(bool);

    ImageFile* emptyThumbnail() const;
    void setEmptyThumbnail(const QString&);

    SlotType slotType() const;
    void setSlotType(SlotType);

    virtual QVariantMap toJsonObject(bool internal=true) const;

protected:
    virtual void loadData(const QVariantMap&, bool internal=false);

signals:

public slots:

private:
    void init();
    int mSlot;
    bool mThumbnailEnabled;
    ImageFile* mEmptyThumbnail;
    SlotType mSlotType;
};

#endif // SLOTBUTTON_H
