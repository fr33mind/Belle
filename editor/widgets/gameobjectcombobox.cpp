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
    for(int i=count()-1; i >= 0; --i) {
        removeItem(i);
    }

    QComboBox::clear();
}

void GameObjectComboBox::setObjects(const QList<GameObject*>& objects, GameObject* selectedObject)
{
    this->clear();
    addObjects(objects);
    setCurrentIndex(-1);
    setCurrentObject(selectedObject);
}

void GameObjectComboBox::addObjects(const QList<GameObject *> & objects)
{
    for(int i=0; i < objects.size(); i++) {
        addObject(objects[i]);
    }
}

void GameObjectComboBox::insertObject(int index, GameObject* object)
{
    if (! object)
        return;

    if (hasIconsEnabled()) {
        const QIcon icon = GameObjectMetaType::icon(object->type());
        insertItem(index, icon, object->name(), QVariant::fromValue(object));
    }
    else
        insertItem(index, object->name(), QVariant::fromValue(object));

    connect(object, SIGNAL(destroyed(GameObject*)), this, SLOT(objectDestroyed(GameObject*)), Qt::UniqueConnection);
}

void GameObjectComboBox::addObject(GameObject* object)
{
    insertObject(count(), object);
}

void GameObjectComboBox::setCurrentObject(GameObject * obj)
{
    int index = findData(QVariant::fromValue(obj));
    if (index != -1)
        setCurrentIndex(index);
}

GameObject* GameObjectComboBox::currentObject() const
{
    QVariant data = currentData();
    if (data.canConvert(QMetaType::QObjectStar))
        return data.value<GameObject*>();
    return 0;
}

void GameObjectComboBox::indexChanged(int index)
{
    QVariant data = itemData(index);
    emit objectChanged(data.value<GameObject*>());
}

void GameObjectComboBox::objectDestroyed(GameObject* object)
{
    removeObject(object);
}

void GameObjectComboBox::removeObject(GameObject * object)
{
    int index = findData(QVariant::fromValue(object));
    if (index != -1) {
        if (currentIndex() == index)
            setCurrentIndex(-1);
        removeItem(index);
    }
}

void GameObjectComboBox::removeItem(int index)
{
    QVariant data = itemData(index);
    if (data.canConvert(QMetaType::QObjectStar)) {
        QObject* obj = data.value<QObject*>();
        if (obj)
            obj->disconnect(this);
    }
    QComboBox::removeItem(index);
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
