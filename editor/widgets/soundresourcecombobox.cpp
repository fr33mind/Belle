#include "soundresourcecombobox.h"

#include <QMessageBox>
#include <QMouseEvent>

SoundResourceComboBox::SoundResourceComboBox(QWidget *parent) :
    GameObjectComboBox(parent)
{
}

void SoundResourceComboBox::mouseReleaseEvent(QMouseEvent *event)
{
    if (!count()) {
        QMessageBox::information(this, tr("No sounds available"), tr("Currently there are no sounds available in your project.\n"
                                                           "Please add them through the Resources section."));
    }

    GameObjectComboBox::mouseReleaseEvent(event);
}
