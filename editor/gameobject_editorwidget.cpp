#include "gameobject_editorwidget.h"

GameObjectEditorWidget::GameObjectEditorWidget(QWidget *parent) :
    PropertiesWidget(parent)
{
    mGameObject = 0;
    mNameEdit = new QLineEdit(this);
    connect(mNameEdit, SIGNAL(textEdited(const QString&)), this, SLOT(onNameEdit(const QString&)));
    beginGroup(tr("GameObject"), "GameObject");
    appendRow("Name", mNameEdit);
    endGroup();
}

void GameObjectEditorWidget::updateData(GameObject* object)
{
    if (object == mGameObject)
        return;

    if (mGameObject)
        mGameObject->disconnect(this);

    if (object)
        connect(object, SIGNAL(destroyed()), this, SLOT(gameObjectDestroyed()));

    mGameObject = object;
    if (! mGameObject)
        return;

    mNameEdit->setText(mGameObject->name());
}

void GameObjectEditorWidget::onNameEdit(const QString& name)
{
    if (mGameObject)
        mGameObject->setName(name);
}

void GameObjectEditorWidget::gameObjectDestroyed()
{
    mGameObject = 0;
}
