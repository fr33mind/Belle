#ifndef FILECOMBOBOX_H
#define FILECOMBOBOX_H

#include "combobox.h"

class FileComboBox : public ComboBox
{
    Q_OBJECT

public:
    enum FileFilter {
        NoFilter,
        ImageFilter,
        SoundFilter
    };

    explicit FileComboBox(QWidget *parent = 0);
    FileComboBox(FileFilter, QWidget *parent = 0);
    void setFileFilter(FileFilter);

signals:
    void fileAdded(const QString&);

private slots:
    void openFileChooser();

private:
    void init();
    FileFilter mFileFilter;

};

#endif // FILECOMBOBOX_H
