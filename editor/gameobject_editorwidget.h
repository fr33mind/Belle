#ifndef GAMEOBJECT_EDITORWIDGET_H
#define GAMEOBJECT_EDITORWIDGET_H

#include <QLineEdit>

#include "properties_widget.h"
#include "gameobject.h"

class GameObject;

class GameObjectEditorWidget : public PropertiesWidget
{
    Q_OBJECT

public:
    explicit GameObjectEditorWidget(QWidget *parent = 0);
    virtual void setGameObject(GameObject*);
    GameObject* gameObject();

protected:
    GameObject* mGameObject;
    QLineEdit* mNameEdit;

signals:
    void gameObjectChanged(GameObject*);

protected:
    virtual void updateData(GameObject*);
    virtual void disconnectGameObject();

public slots:
    void reload();

protected slots:
    virtual void onGameObjectDestroyed();

private slots:
    void onNameEdited(const QString&);
    void onGameObjectNameChanged(const QString&);
};

#endif // GAMEOBJECT_EDITORWIDGET_H
