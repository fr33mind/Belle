#include "object_combobox.h"

ObjectComboBox::ObjectComboBox(QWidget *parent) :
    QComboBox(parent)
{
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
}

void ObjectComboBox::clear()
{
    QComboBox::clear();
    for(int i=0; i < mObjects.size(); i++) {
        mObjects[i]->disconnect(this);
    }
    mObjects.clear();
}

void ObjectComboBox::loadFromScene(Scene * scene)
{
    this->clear();
    if (scene)
        this->setObjects(scene->objects());
}

void ObjectComboBox::loadFromAction(Action * action)
{
    if (! action)
        return;

    this->blockSignals(true);
    this->clear();
    int currIndex = 0, pindex = 0;

    this->loadFromScene(action->scene());

    if (action->sceneObject()) {
        currIndex = mObjects.indexOf(action->sceneObject());
        if (currIndex == -1) {
            insertObject(0, action->sceneObject());
            currIndex = 0;
        }
    }

    Object* parent = qobject_cast<Object*>(action->parent());
    if (parent) {
        pindex = mObjects.indexOf(parent);
        if (pindex == -1) {
            insertObject(0, parent);
            pindex = currIndex = 0;
        }

        if (pindex != -1) {
            this->setItemText(pindex, QString("%1 (%2)").arg(parent->name())
                                                       .arg(tr("parent")));
        }
    }

    this->blockSignals(false);

    setCurrentIndex(currIndex);
    if (! action->sceneObject() && ! mObjects.isEmpty())
        action->setSceneObject(mObjects.first());
}

void ObjectComboBox::setObjects(const QList<Object*>& objects)
{
    this->clear();
    for(int i=0; i < objects.size(); i++) {
        addObject(objects[i]);
    }
}

void ObjectComboBox::insertObject(int index, Object* object)
{
    if (! object)
        return;

    addItem(object->objectName());
    mObjects.insert(index, object);
    connect(object, SIGNAL(destroyed(Object*)), this, SLOT(objectDestroyed(Object*)), Qt::UniqueConnection);
}

void ObjectComboBox::addObject(Object* object)
{
    insertObject(count(), object);
}


Object* ObjectComboBox::currentObject() const
{
    int currIndex = this->currentIndex();
    if (currIndex >= 0 && currIndex < mObjects.size())
        return mObjects[currIndex];
    return 0;
}

void ObjectComboBox::indexChanged(int index)
{
    if (index >= 0 && index < mObjects.size())
        emit objectChanged(mObjects[index]);
}

void ObjectComboBox::objectDestroyed(Object* object)
{
    int index = mObjects.indexOf(object);
    if (index != -1)
        mObjects.removeAt(index);
}
