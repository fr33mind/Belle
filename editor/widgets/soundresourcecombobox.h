#ifndef SOUNDRESOURCECOMBOBOX_H
#define SOUNDRESOURCECOMBOBOX_H

#include "gameobjectcombobox.h"

class SoundResourceComboBox : public GameObjectComboBox
{
    Q_OBJECT
public:
    explicit SoundResourceComboBox(QWidget *parent = 0);

protected:
    virtual void mouseReleaseEvent(QMouseEvent *event);

signals:

public slots:

};

#endif // SOUNDRESOURCECOMBOBOX_H
