#include "changestate.h"

#include "scene.h"

static ChangeStateEditorWidget * mEditorWidget = 0;
ActionInfo ChangeState::Info;

ChangeState::ChangeState(QObject *parent) :
    Action(parent)
{
    init();
}

ChangeState::ChangeState(const QVariantMap& data, QObject *parent) :
    Action(data, parent)
{
    init();

    if (data.contains("state") && data.value("state").type() == QVariant::String)
        setState(data.value("state").toString());
}

void ChangeState::init()
{
    setTypeName(Info.typeName);
    setType(Info.type);
    setIcon(Info.icon);
    mState = "";
}

void ChangeState::setChangeStateEditorWidget(ChangeStateEditorWidget * widget)
{
   mEditorWidget = widget;
}

ChangeStateEditorWidget* ChangeState::changeStateEditorWidget()
{
   return mEditorWidget;
}

ActionEditorWidget* ChangeState::editorWidget()
{
    return mEditorWidget;
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
        return scene->objects("Character");

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

QVariantMap ChangeState::toJsonObject()
{
    QVariantMap action = Action::toJsonObject();
    action.insert("state", mState);
    return action;
}
