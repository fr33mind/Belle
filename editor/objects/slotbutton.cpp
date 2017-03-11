#include "slotbutton.h"

#include "image.h"
#include "textbox.h"

SlotButton::SlotButton(QObject *parent) :
    ObjectGroup(parent)
{
    init();
    Image* img = new Image("", this);
    img->setName("thumbnail");
    img->setNameEditable(false);
    img->setKeepAspectRatio(false);
    img->setWidth(50);
    img->setHeight(50);
    add(img, 0, 0);

    TextBox* textbox = new TextBox("$id. Empty Slot", this);
    textbox->setName("textbox");
    textbox->setNameEditable(false);
    textbox->setWidth(200);
    textbox->setHeight(img->height());
    textbox->setTextAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    add(textbox, img->width()+10, 0);
}

SlotButton::SlotButton(const QVariantMap& data, QObject *parent) :
    ObjectGroup(data, parent)
{
    init();
    loadInternal(data);
}

SlotButton::~SlotButton()
{
    if (mEmptyThumbnail)
        AssetManager::instance()->releaseAsset(mEmptyThumbnail);

    mEmptyThumbnail = 0;
}

void SlotButton::init()
{
    setType(GameObjectMetaType::SlotButton);
    mSlot = 0;
    mThumbnailEnabled = true;
    mEmptyThumbnail = 0;
    mSlotType = SlotButton::Save;
    setAlignEnabled(false);
}

void SlotButton::loadData(const QVariantMap& data, bool internal)
{
    if (!internal)
        ObjectGroup::loadData(data, internal);

    if (data.contains("slot") && data.value("slot").canConvert(QVariant::Int)) {
        setSlot(data.value("slot").toInt());
    }

    if (data.contains("thumbnailEnabled") && data.value("thumbnailEnabled").type() == QVariant::Bool) {
        setThumbnailEnabled(data.value("thumbnailEnabled").toBool());
    }

    if (data.contains("emptyThumbnail") && data.value("emptyThumbnail").type() == QVariant::String) {
        setEmptyThumbnail(data.value("emptyThumbnail").toString());
    }

    if (data.contains("slotType") && data.value("slotType").canConvert(QVariant::Int)) {
        setSlotType(static_cast<SlotType>(data.value("slotType").toInt()));
    }
}

int SlotButton::slot() const
{
    return mSlot;
}

void SlotButton::setSlot(int slot)
{
    mSlot = slot;
}

Image* SlotButton::thumbnail() const
{
    return qobject_cast<Image*>(object("thumbnail"));
}

bool SlotButton::thumbnailEnabled() const
{
    return mThumbnailEnabled;
}

void SlotButton::setThumbnailEnabled(bool enabled)
{
    mThumbnailEnabled = enabled;
    Image* thumb = thumbnail();
    if (thumb)
        thumb->setVisible(mThumbnailEnabled);
    notify("thumbnailEnabled", mThumbnailEnabled);
}

ImageFile* SlotButton::emptyThumbnail() const
{
    return mEmptyThumbnail;
}

void SlotButton::setEmptyThumbnail(const QString & thumb)
{
    ImageFile* image = dynamic_cast<ImageFile*>(AssetManager::instance()->loadAsset(thumb, Asset::Image));
    if (mEmptyThumbnail == image) {
        AssetManager::instance()->releaseAsset(image);
        return;
    }

    mEmptyThumbnail = image;

    Image* thumbImage = thumbnail();
    if (thumbImage)
        thumbImage->setImage(mEmptyThumbnail);

    QString name;
    if (mEmptyThumbnail)
        name = mEmptyThumbnail->name();
    notify("emptyThumbnail", name);
}

SlotButton::SlotType SlotButton::slotType() const
{
    return mSlotType;
}

void SlotButton::setSlotType(SlotType type)
{
    mSlotType = type;
}

QVariantMap SlotButton::toJsonObject(bool internal) const
{
    QVariantMap data = ObjectGroup::toJsonObject(internal);
    data.insert("slot", mSlot);
    data.insert("thumbnailEnabled", mThumbnailEnabled);
    if (mEmptyThumbnail)
        data.insert("emptyThumbnail", mEmptyThumbnail->name());
    data.insert("slotType", mSlotType);
    return data;
}

void SlotButton::loadObject(Object * obj, const QVariantMap & data)
{
    if (!obj)
        return;

    ObjectGroup::loadObject(obj, data);

    TextBox* textbox = qobject_cast<TextBox*>(obj);
    if (textbox) {
        if (data.contains("text") && data.value("text").type() == QVariant::String)
            textbox->setText(data.value("text").toString());
    }
}

void SlotButton::paint(QPainter & painter)
{
    ObjectGroup::paint(painter);

    if (thumbnailEnabled()) {
        Image* thumb = thumbnail();
        if (thumb && !thumb->image() && (!thumb->borderWidth() || !thumb->borderColor().isValid())) {
            QRect rect = thumb->sceneRect();
            QPen pen(Qt::white);
            painter.save();
            painter.setOpacity(0.5);
            pen.setWidth(1);
            painter.setPen(pen);
            painter.drawRect(rect);
            painter.restore();
        }
    }
}
