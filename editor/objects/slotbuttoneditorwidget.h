#ifndef SLOTBUTTONEDITORWIDGET_H
#define SLOTBUTTONEDITORWIDGET_H

#include <QCheckBox>
#include "objectgroup_editor_widget.h"
#include "choosefilebutton.h"

class SlotButtonEditorWidget : public ObjectGroupEditorWidget
{
    Q_OBJECT
public:
    explicit SlotButtonEditorWidget(QWidget *parent = 0);

protected:
    virtual void updateData(GameObject*);

signals:

private slots:
    void onSlotChanged(int);
    void onThumbnailToggled(bool);
    void onEmptyThumbnailChosen(const QString&);
    void onSlotTypeChanged(int);

private:
    QSpinBox* mSlotSpinBox;
    QCheckBox* mThumbnailCheckBox;
    ChooseFileButton* mEmptyThumbnailButton;
    QComboBox* mSlotTypeComboBox;

};

#endif // SLOTBUTTONEDITORWIDGET_H
