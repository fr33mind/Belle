#include "gameobjectcombobox.h"

GameObjectComboBox::GameObjectComboBox(QWidget *parent) :
    QComboBox(parent)
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
    mTypeFilter = GameObjectMetaType::UnknownType;
    mTypeFilterActive = false;
}

void GameObjectComboBox::clear()
{
    QComboBox::clear();
    for(int i=0; i < mObjects.size(); i++) {
        mObjects[i]->disconnect(this);
    }
    mObjects.clear();
}

void GameObjectComboBox::setObjects(const QList<GameObject*>& objects, GameObject* selectedObject)
{
    this->clear();
    int currIndex = -1;

    for(int i=0; i < objects.size(); i++) {
        addObject(objects[i]);
        if (selectedObject && objects[i] == selectedObject)
            currIndex = count() - 1;
    }

    setCurrentIndex(currIndex);
}

void GameObjectComboBox::insertObject(int index, GameObject* object)
{
    if (! object)
        return;


    if (hasIconsEnabled()) {
        const QIcon icon = GameObjectMetaType::icon(object->type());
        insertItem(index, icon, object->name());
    }
    else
        insertItem(index, object->name());

    mObjects.insert(index, object);
    connect(object, SIGNAL(destroyed(GameObject*)), this, SLOT(objectDestroyed(GameObject*)), Qt::UniqueConnection);
}

void GameObjectComboBox::addObject(GameObject* object)
{
    insertObject(count(), object);
}

GameObject* GameObjectComboBox::currentObject() const
{
    int currIndex = this->currentIndex();
    if (currIndex >= 0 && currIndex < mObjects.size())
        return mObjects[currIndex];
    return 0;
}

void GameObjectComboBox::indexChanged(int index)
{
    if (index >= 0 && index < mObjects.size())
        emit objectChanged(mObjects[index]);
    else
        emit objectChanged(0);
}

void GameObjectComboBox::objectDestroyed(GameObject* object)
{
    int index = mObjects.indexOf(object);
    if (index != -1) {
        mObjects.removeAt(index);
        if (currentIndex() == index)
            setCurrentIndex(-1);
        removeItem(index);
    }
}

void GameObjectComboBox::setIconsEnabled(bool enabled)
{
    mIconsEnabled = enabled;
}

bool GameObjectComboBox::hasIconsEnabled() const
{
    return mIconsEnabled;
}

void GameObjectComboBox::setTypeFilter(GameObjectMetaType::Type type)
{
    if (mTypeFilter != type) {
        mTypeFilter = type;
        setTypeFilterActive(true);
    }
}

GameObjectMetaType::Type GameObjectComboBox::typeFilter() const
{
    return mTypeFilter;
}

void GameObjectComboBox::setTypeFilterActive(bool active)
{
    mTypeFilterActive = active;
}

bool GameObjectComboBox::isTypeFilterActive() const
{
    return mTypeFilterActive;
}
