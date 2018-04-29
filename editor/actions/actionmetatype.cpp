#include "actionmetatype.h"

static QColor mStoryColor;
static QColor mTransformColor;
static QColor mAudioColor;
static QColor mScriptColor;

ActionMetaType::ActionMetaType(QObject *parent) :
    QObject(parent)
{
}

void ActionMetaType::init()
{
    mStoryColor = QColor("#FFB9B9");
    mTransformColor = QColor("#D4B9FF");
    mAudioColor = QColor("#C6E5A6");
    mScriptColor = QColor("#CCCCCC");
}

QColor ActionMetaType::color(GameObjectMetaType::Type type)
{
    switch(type) {
    case GameObjectMetaType::Dialogue:
    case GameObjectMetaType::Wait:
    case GameObjectMetaType::Show:
    case GameObjectMetaType::Hide:
    case GameObjectMetaType::ChangeBackground:
    case GameObjectMetaType::ChangeState:
    case GameObjectMetaType::Label:
    case GameObjectMetaType::GoToLabel:
    case GameObjectMetaType::GoToScene:
    case GameObjectMetaType::Branch:
    case GameObjectMetaType::ShowMenu:
    case GameObjectMetaType::End:
        return mStoryColor;
    case GameObjectMetaType::Slide:
    case GameObjectMetaType::Fade:
    case GameObjectMetaType::ChangeObjectBackground:
        return mTransformColor;
    case GameObjectMetaType::PlaySound:
    case GameObjectMetaType::StopSound:
    case GameObjectMetaType::StopAllSounds:
        return mAudioColor;
    case GameObjectMetaType::GetUserInput:
    case GameObjectMetaType::SetGameVariable:
    case GameObjectMetaType::RunScript:
        return mScriptColor;
    default:
        return QColor();
    }
}

QColor ActionMetaType::color(const Action * action)
{
    if (! action)
        return QColor();
    return color(action->type());
}
