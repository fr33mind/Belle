#include "soundeditorwidget.h"

#include "sound.h"

SoundEditorWidget::SoundEditorWidget(QWidget *parent) :
    GameObjectEditorWidget(parent)
{
    mFileComboBox = new FileComboBox(FileComboBox::SoundFilter, this);
    connect(mFileComboBox, SIGNAL(fileAdded(const QString&)), this, SLOT(onFileAdded(const QString&)));
    connect(mFileComboBox, SIGNAL(itemRemoved(int)), this, SLOT(onItemRemoved(int)));
    beginGroup("Sound");
    appendRow(tr("Files"), mFileComboBox);
    endGroup();
}

void SoundEditorWidget::updateData(GameObject* obj)
{
    GameObjectEditorWidget::updateData(obj);
    Sound* sound = qobject_cast<Sound*>(obj);

    if (! sound)
        return;

    mFileComboBox->clear();
    foreach(Asset* asset, sound->assets()) {
        mFileComboBox->addItem(GameObjectMetaType::icon(GameObjectMetaType::Sound), asset->name(), asset->path());
    }
}

void SoundEditorWidget::onFileAdded(const QString & path)
{
    Sound* sound = qobject_cast<Sound*>(mGameObject);
    if (!sound)
        return;

    sound->addFile(path);
}

void SoundEditorWidget::onItemRemoved(int index)
{
    Sound* sound = qobject_cast<Sound*>(mGameObject);
    if (!sound)
        return;

    sound->removeAssetAt(index);
}
