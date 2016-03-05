#include "gameobject_editorwidget.h"

GameObjectEditorWidget::GameObjectEditorWidget(QWidget *parent) :
    PropertiesWidget(parent)
{
    mGameObject = 0;
    mNameEdit = new QLineEdit(this);
    connect(mNameEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onNameEdited(const QString&)));
    beginGroup(tr("GameObject"), "GameObject");
    appendRow("Name", mNameEdit);
    endGroup();
}

void GameObjectEditorWidget::updateData(GameObject* object)
{
    mNameEdit->setStyleSheet("background: transparent");
    mNameEdit->setText(object->name());
}

void GameObjectEditorWidget::onNameEdited(const QString& name)
{
    if (mGameObject) {
        bool ok = mGameObject->setName(name);
        if (ok)
            mNameEdit->setStyleSheet("background: transparent");
        else
            mNameEdit->setStyleSheet("background: #FF6666");
    }
}

void GameObjectEditorWidget::onGameObjectDestroyed()
{
    mGameObject = 0;
}

void GameObjectEditorWidget::setGameObject(GameObject * object)
{
    if (object == mGameObject) {
        reload();
        return;
    }

    if (mGameObject)
        mGameObject->disconnect(this);
    mGameObject = 0;

    if (! object)
        return;

    connect(object, SIGNAL(destroyed()), this, SLOT(onGameObjectDestroyed()), Qt::UniqueConnection);
    updateData(object);
    mGameObject = object;
    emit gameObjectChanged(mGameObject);
}

GameObject* GameObjectEditorWidget::gameObject()
{
    return mGameObject;
}

void GameObjectEditorWidget::reload()
{
    if (! mGameObject)
        return;

    GameObject* obj = mGameObject;
    mGameObject = 0;
    updateData(obj);
    mGameObject = obj;
}
