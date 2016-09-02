#include "filecombobox.h"
#include "utils.h"
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

    switch(mFileFilter) {
    case ImageFilter:
        path = FileDialogUtils::getOpenImageFileName(this);
        break;
    case SoundFilter:
        path = FileDialogUtils::getOpenSoundFileName(this);
        break;
    default:
        path = QFileDialog::getOpenFileName(this);
        break;
    }

    if (path.isEmpty() || findData(path) != -1) {
        return;
    }

    addItem(QIcon(":/media/sound.png"), QFileInfo(path).fileName(), path);
    emit fileAdded(path);
}
