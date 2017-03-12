#include "slotbuttoneditorwidget.h"

#include <QHBoxLayout>
#include "filecombobox.h"
#include "slotbutton.h"

SlotButtonEditorWidget::SlotButtonEditorWidget(QWidget *parent) :
    ObjectGroupEditorWidget(parent)
{
    mSlotSpinBox = new QSpinBox(this);
    QWidget* thumbnailWidget = new QWidget(this);
    mThumbnailCheckBox = new QCheckBox(thumbnailWidget);
    mThumbnailCheckBox->setChecked(true);
    mEmptyThumbnailButton = new ChooseFileButton(ChooseFileButton::ImageFilter, thumbnailWidget);
    mEmptyThumbnailButton->setPlaceholderText(tr("Empty Thumbnail"));
    mEmptyThumbnailButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout* layout = new QHBoxLayout(thumbnailWidget);
    layout->setContentsMargins(0, 1, 0, 1);
    layout->addWidget(mThumbnailCheckBox, 0, Qt::AlignLeft);
    layout->addWidget(mEmptyThumbnailButton);

    mSlotTypeComboBox = new QComboBox(this);
    mSlotTypeComboBox->addItem(tr("Save"));
    mSlotTypeComboBox->addItem(tr("Load"));

    beginGroup(tr("Slot Button"), "SlotButton");
    appendRow(tr("Slot"), mSlotSpinBox);
    appendRow(tr("Thumbnail"), thumbnailWidget);
    appendRow(tr("Slot Type"), mSlotTypeComboBox);
    endGroup();
    //Fixes height issue that happens when button is not visible
    thumbnailWidget->setFixedHeight(thumbnailWidget->height());

    connect(mThumbnailCheckBox, SIGNAL(toggled(bool)), mEmptyThumbnailButton, SLOT(setVisible(bool)));
    connect(mSlotSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onSlotChanged(int)));
    connect(mThumbnailCheckBox, SIGNAL(toggled(bool)), this, SLOT(onThumbnailToggled(bool)));
    connect(mEmptyThumbnailButton, SIGNAL(fileSelected(const QString&)), this, SLOT(onEmptyThumbnailChosen(const QString&)));
    connect(mSlotTypeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onSlotTypeChanged(int)));
}

void SlotButtonEditorWidget::updateData(GameObject* object)
{
    ObjectGroupEditorWidget::updateData(object);
    SlotButton* slotButton = qobject_cast<SlotButton*>(object);
    if (! slotButton)
        return;

    mSlotSpinBox->setValue(slotButton->slot());
    mThumbnailCheckBox->setChecked(slotButton->thumbnailEnabled());
    mEmptyThumbnailButton->setImageFile(slotButton->emptyThumbnail());
    int index = static_cast<int>(slotButton->slotType());
    mSlotTypeComboBox->setCurrentIndex(index);
}

void SlotButtonEditorWidget::onSlotChanged(int slot)
{
    SlotButton* slotButton = qobject_cast<SlotButton*>(mGameObject);
    if (! slotButton)
        return;

    slotButton->setSlot(slot);
}

void SlotButtonEditorWidget::onThumbnailToggled(bool checked)
{
    SlotButton* slotButton = qobject_cast<SlotButton*>(mGameObject);
    if (! slotButton)
        return;

    slotButton->setThumbnailEnabled(checked);
}

void SlotButtonEditorWidget::onEmptyThumbnailChosen(const QString& path)
{
    SlotButton* slotButton = qobject_cast<SlotButton*>(mGameObject);
    if (! slotButton)
        return;

    slotButton->setEmptyThumbnail(path);
}

void SlotButtonEditorWidget::onSlotTypeChanged(int index)
{
    SlotButton* slotButton = qobject_cast<SlotButton*>(mGameObject);
    if (! slotButton)
        return;

    slotButton->setSlotType(static_cast<SlotButton::SlotType>(index));
}
