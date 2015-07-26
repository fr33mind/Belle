#ifndef GAMEOBJECT_EDITORWIDGET_H
#define GAMEOBJECT_EDITORWIDGET_H

#include <QLineEdit>

#include "properties_widget.h"
#include "gameobject.h"

class GameObject;

class GameObjectEditorWidget : public PropertiesWidget
{
    Q_OBJECT

    GameObject* mGameObject;
    QLineEdit* mNameEdit;

public:
    explicit GameObjectEditorWidget(QWidget *parent = 0);
    virtual void updateData(GameObject*);

signals:

public slots:
    void onNameEdit(const QString&);

protected slots:
    void gameObjectDestroyed();

};

#endif // GAMEOBJECT_EDITORWIDGET_H
