#include "stopallsoundseditorwidget.h"
#include "stopallsounds.h"

StopAllSoundsEditorWidget::StopAllSoundsEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mFadeOutSpinBox = new QDoubleSpinBox(this);
    mFadeOutSpinBox->setMaximum(10);
    mFadeOutSpinBox->setSingleStep(0.1);

    beginGroup(tr("Stop All Sounds"));
    appendRow(QString("%1 %2").arg(tr("Fade out duration")).arg("(s)") , mFadeOutSpinBox);
    endGroup();

    connect(mFadeOutSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onFadeChanged(double)));
    resizeColumnToContents(0);
}

void StopAllSoundsEditorWidget::updateData(GameObject* action)
{
    ActionEditorWidget::updateData(action);
    StopAllSounds * stopAllSounds = qobject_cast<StopAllSounds*>(action);
    if (! stopAllSounds)
        return;

    mFadeOutSpinBox->setValue(stopAllSounds->fadeTime());
}

void StopAllSoundsEditorWidget::onFadeChanged(double val)
{
    StopAllSounds * stopAllSounds = qobject_cast<StopAllSounds*>(mGameObject);
    if (stopAllSounds)
        stopAllSounds->setFadeTime(val);
}
