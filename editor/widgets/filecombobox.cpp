#include "filecombobox.h"
#include "utils.h"
#include "gameobjectmetatype.h"
#include <QFileDialog>

FileComboBox::FileComboBox(QWidget *parent) :
    ComboBox(parent)
{
    init();
}

FileComboBox::FileComboBox(FileFilter filter, QWidget *parent) :
    ComboBox(parent)
{
    init();
    setFileFilter(filter);
}

void FileComboBox::setFileFilter(FileFilter filter)
{
    mFileFilter = filter;
}

void FileComboBox::init()
{
    mFileFilter = NoFilter;
    connect(this, SIGNAL(addItemActivated()), this, SLOT(openFileChooser()));
}

void FileComboBox::openFileChooser()
{
    QString path;
    QIcon icon;

    switch(mFileFilter) {
    case ImageFilter:
        path = FileDialogUtils::getOpenImageFileName(this);
        icon = GameObjectMetaType::icon(GameObjectMetaType::Image);
        break;
    case SoundFilter:
        path = FileDialogUtils::getOpenSoundFileName(this);
        icon = GameObjectMetaType::icon(GameObjectMetaType::Sound);
        break;
    case FontFilter:
        path = FileDialogUtils::getOpenFontFileName(this);
        icon = GameObjectMetaType::icon(GameObjectMetaType::Font);
        break;
    default:
        path = QFileDialog::getOpenFileName(this);
        break;
    }

    if (path.isEmpty() || findData(path) != -1) {
        return;
    }

    addItem(icon, QFileInfo(path).fileName(), path);
    emit fileAdded(path);
}
