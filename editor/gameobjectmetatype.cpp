#include "gameobjectmetatype.h"

#include <QHash>

static QHash<GameObjectMetaType::Type, GameObjectMetaType*> mGameObjectMetaTypes;

GameObjectMetaType::GameObjectMetaType(GameObjectMetaType::Type type, const QString& typeString,
                                       const QString& name, const QIcon& icon, QObject *parent) :
    QObject(parent)
{
    mType = type;
    mTypeString = typeString;
    mName = name;
    mIcon = icon;
}

const QIcon& GameObjectMetaType::icon() const
{
    return mIcon;
}

QString GameObjectMetaType::name() const
{
    return mName;
}

GameObjectMetaType::Type GameObjectMetaType::type() const
{
    return mType;
}

QString GameObjectMetaType::toString() const
{
    return mTypeString;
}

void GameObjectMetaType::init()
{
    if (! mGameObjectMetaTypes.isEmpty())
        return;

    mGameObjectMetaTypes.insert(GameObject, new GameObjectMetaType(GameObject, "GameObject", tr("GameObject")));

    //objects
    mGameObjectMetaTypes.insert(Object, new GameObjectMetaType(Object, "Object", tr("Object")));
    mGameObjectMetaTypes.insert(TextBox, new GameObjectMetaType(TextBox, "TextBox", tr("Text Box"), QIcon(":/media/text.png")));
    mGameObjectMetaTypes.insert(Button, new GameObjectMetaType(Button, "Button", tr("Button"), QIcon(":/media/button.png")));
    mGameObjectMetaTypes.insert(Image, new GameObjectMetaType(Image, "Image", tr("Image"), QIcon(":/media/image.png")));
    mGameObjectMetaTypes.insert(Character, new GameObjectMetaType(Character, "Character", tr("Character"), QIcon(":/media/user.png")));
    mGameObjectMetaTypes.insert(ObjectGroup, new GameObjectMetaType(ObjectGroup, "ObjectGroup", tr("ObjectGroup")));
    mGameObjectMetaTypes.insert(DialogueBox, new GameObjectMetaType(DialogueBox, "DialogueBox", tr("Dialogue Box"), QIcon(":/media/talk-baloon.png")));
    mGameObjectMetaTypes.insert(Menu, new GameObjectMetaType(Menu, "Menu", tr("Menu")));
    mGameObjectMetaTypes.insert(MenuOption, new GameObjectMetaType(MenuOption, "MenuOption", tr("Menu Option")));
    mGameObjectMetaTypes.insert(Sound, new GameObjectMetaType(Sound, "Sound", tr("Sound"), QIcon(":/media/sound.png")));

    //scene
    mGameObjectMetaTypes.insert(Scene, new GameObjectMetaType(Scene, "Scene", tr("Scene")));

    //actions
    mGameObjectMetaTypes.insert(Action, new GameObjectMetaType(Action, "Action", tr("Action")));
    mGameObjectMetaTypes.insert(Show, new GameObjectMetaType(Show, "Show", tr("Show"), QIcon(":/media/show.png")));
    mGameObjectMetaTypes.insert(Hide, new GameObjectMetaType(Hide, "Hide", tr("Hide"), QIcon(":/media/hide.png")));
    mGameObjectMetaTypes.insert(Wait, new GameObjectMetaType(Wait, "Wait", tr("Wait"), QIcon(":/media/timer.png")));
    mGameObjectMetaTypes.insert(Dialogue, new GameObjectMetaType(Dialogue, "Dialogue", tr("Dialogue"), QIcon(":/media/talk-baloon.png")));
    mGameObjectMetaTypes.insert(Slide, new GameObjectMetaType(Slide, "Slide", tr("Slide"), QIcon(":/media/slide.png")));
    mGameObjectMetaTypes.insert(Fade, new GameObjectMetaType(Fade, "Fade", tr("Fade"), QIcon(":/media/fade.png")));
    mGameObjectMetaTypes.insert(Label, new GameObjectMetaType(Label, "Label", tr("Label"), QIcon(":/media/label.png")));
    mGameObjectMetaTypes.insert(GoToLabel, new GameObjectMetaType(GoToLabel, "GoToLabel", tr("Go To Label"), QIcon(":/media/go-to-label.png")));
    mGameObjectMetaTypes.insert(GoToScene, new GameObjectMetaType(GoToScene, "GoToScene", tr("Go To Scene"), QIcon(":/media/go-to-scene.png")));
    mGameObjectMetaTypes.insert(Branch, new GameObjectMetaType(Branch, "Branch", tr("Branch"), QIcon(":/media/branch.png")));
    mGameObjectMetaTypes.insert(GetUserInput, new GameObjectMetaType(GetUserInput, "GetUserInput", tr("Get User Input"), QIcon(":/media/edit-clear.png")));
    mGameObjectMetaTypes.insert(ShowMenu, new GameObjectMetaType(ShowMenu, "ShowMenu", tr("Show Menu"), QIcon(":/media/menu.png")));
    mGameObjectMetaTypes.insert(ChangeColor, new GameObjectMetaType(ChangeColor, "ChangeColor", tr("Change Color"), QIcon(":/media/color.png")));
    mGameObjectMetaTypes.insert(PlaySound, new GameObjectMetaType(PlaySound, "PlaySound", tr("Play Sound"), QIcon(":/media/sound.png")));
    mGameObjectMetaTypes.insert(StopSound, new GameObjectMetaType(StopSound, "StopSound", tr("Stop Sound"), QIcon(":/media/no-sound.png")));
    mGameObjectMetaTypes.insert(SetGameVariable, new GameObjectMetaType(SetGameVariable, "SetGameVariable",
                                                                           tr("Set Game Variable"), QIcon(":/media/script.png")));
    mGameObjectMetaTypes.insert(ChangeBackground, new GameObjectMetaType(ChangeBackground, "ChangeBackground", tr("Change Background"), QIcon(":/media/image.png")));
    mGameObjectMetaTypes.insert(RunScript, new GameObjectMetaType(RunScript, "RunScript", tr("Run Script"), QIcon(":/media/script.png")));
    mGameObjectMetaTypes.insert(ChangeState, new GameObjectMetaType(ChangeState, "ChangeState", tr("Change State"), QIcon(":/media/user-group.png")));
    mGameObjectMetaTypes.insert(End, new GameObjectMetaType(End, "End", tr("End"), QIcon(":/media/end-novel.png")));
}

void GameObjectMetaType::destroy()
{
    QHashIterator<GameObjectMetaType::Type, GameObjectMetaType*> it(mGameObjectMetaTypes);

    while(it.hasNext()) {
        it.next();
        GameObjectMetaType* metaType = it.value();
        if (metaType)
            metaType->deleteLater();
    }

    mGameObjectMetaTypes.clear();
}

const GameObjectMetaType* GameObjectMetaType::metaType(Type type)
{
    return mGameObjectMetaTypes.value(type, 0);
}

const QIcon& GameObjectMetaType::icon(Type type)
{
    const GameObjectMetaType* metaType = GameObjectMetaType::metaType(type);
    if (metaType)
        return metaType->icon();
    return QIcon();
}

GameObjectMetaType::Type GameObjectMetaType::typeFromString(const QString& typeString)
{
    QHashIterator<GameObjectMetaType::Type, GameObjectMetaType*> it(mGameObjectMetaTypes);
    QString typeStringLower = typeString.toLower();

    //For backwards compatibility. Remove at some point.
    if(typeStringLower == "changegamevariable")
        typeStringLower = "setgamevariable";

    while(it.hasNext()) {
        it.next();
        GameObjectMetaType* metaType = it.value();
        if (metaType && metaType->toString().toLower() == typeStringLower) {
            return metaType->type();
        }
    }

    return GameObjectMetaType::UnknownType;
}
