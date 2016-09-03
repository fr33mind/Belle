#ifndef SOUNDEDITORWIDGET_H
#define SOUNDEDITORWIDGET_H

#include "gameobject_editorwidget.h"
#include "filecombobox.h"

class SoundEditorWidget : public GameObjectEditorWidget
{
    Q_OBJECT
public:
    explicit SoundEditorWidget(QWidget *parent = 0);

protected:
    virtual void updateData(GameObject*);

signals:

private slots:
    void onFileAdded(const QString&);
    void onItemRemoved(int);

private:
    FileComboBox* mFileComboBox;

};

#endif // SOUNDEDITORWIDGET_H
