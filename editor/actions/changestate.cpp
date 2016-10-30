#include "changestate.h"

#include "scene.h"

ChangeState::ChangeState(QObject *parent) :
    Action(parent)
{
    init();
}

ChangeState::ChangeState(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();
    loadInternal(data);
}

void ChangeState::init()
{
    setType(GameObjectMetaType::ChangeState);
    mState = "";
}

void ChangeState::loadData(const QVariantMap & data, bool internal)
{
    if (!internal)
        Action::loadData(data, internal);

    if (data.contains("state") && data.value("state").type() == QVariant::String)
        setState(data.value("state").toString());
}

QString ChangeState::displayText() const
{
    if (sceneObject() && ! mState.isEmpty())
        return QString("\"%1\" to \"%2\"").arg(sceneObject()->name()).arg(mState);
    return "";
}

QList<Object*> ChangeState::availableObjects() const
{
    Scene* scene = this->scene();
    if (scene)
        return scene->objects(GameObjectMetaType::Character);

    return QList<Object*>();
}

QString ChangeState::state() const
{
    return mState;
}

void ChangeState::setState(const QString & state)
{
    if (mState == state)
        return;

    mState = state;
    emit dataChanged();
}

QVariantMap ChangeState::toJsonObject(bool internal) const
{
    QVariantMap action = Action::toJsonObject(internal);
    action.insert("state", mState);
    return action;
}
