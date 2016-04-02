#include "gameobjectfactory.h"

#include "image.h"
#include "character.h"
#include "dialoguebox.h"
#include "textbox.h"
#include "button.h"
#include "menu.h"
#include "scene.h"
#include "wait.h"
#include "dialogue.h"
#include "label.h"
#include "gotolabel.h"
#include "gotoscene.h"
#include "slide.h"
#include "fade.h"
#include "show.h"
#include "hide.h"
#include "branch.h"
#include "get_user_input.h"
#include "changecolor.h"
#include "playsound.h"
#include "stopsound.h"
#include "show_menu.h"
#include "end.h"
#include "change_game_variable.h"
#include "change_background.h"
#include "runscript.h"
#include "changestate.h"

void GameObjectFactory::init()
{
    GameObjectMetaType::init();
}

void GameObjectFactory::destroy()
{
    GameObjectMetaType::destroy();
}

GameObject* GameObjectFactory::createGameObject(GameObjectMetaType::Type type, const QVariantMap & data, QObject * parent)
{
    GameObject* obj = 0;

    if (type >= GameObjectMetaType::firstActionType && type <= GameObjectMetaType::lastActionType)
        obj = createAction(type, data, parent);
    else if (type >= GameObjectMetaType::firstObjectType && type <= GameObjectMetaType::lastObjectType)
        obj = createObject(type, data, parent);
    else if (type == GameObjectMetaType::Scene)
        obj = createScene(data, parent);

    return obj;
}

GameObject* GameObjectFactory::createGameObject(const QVariantMap & data, QObject * parent)
{
    GameObjectMetaType::Type type = GameObjectMetaType::typeFromString(data.value("type").toString());
    return createGameObject(type, data, parent);
}

Action* GameObjectFactory::createAction(GameObjectMetaType::Type type, const QVariantMap & data, QObject * parent)
{
    Action* action = 0;

    switch(type) {
        case GameObjectMetaType::Dialogue: action = new Dialogue(data, parent); break;
        case GameObjectMetaType::Wait: action = new Wait(data, parent); break;
        case GameObjectMetaType::Show: action = new Show(data, parent); break;
        case GameObjectMetaType::Hide: action = new Hide(data, parent); break;
        case GameObjectMetaType::ChangeBackground: action = new ChangeBackground(data, parent); break;
        case GameObjectMetaType::ChangeState: action = new ChangeState(data, parent); break;
        case GameObjectMetaType::Label: action = new Label(data, parent); break;
        case GameObjectMetaType::GoToLabel: action = new GoToLabel(data, parent); break;
        case GameObjectMetaType::GoToScene: action = new GoToScene(data, parent); break;
        case GameObjectMetaType::ShowMenu: action = new ShowMenu(data, parent); break;
        case GameObjectMetaType::Slide: action = new Slide(data, parent); break;
        case GameObjectMetaType::Fade: action = new Fade(data, parent); break;
        case GameObjectMetaType::Branch: action = new Branch(data, parent); break;
        case GameObjectMetaType::ChangeColor: action = new ChangeColor(data, parent); break;
        case GameObjectMetaType::PlaySound: action = new PlaySound(data, parent); break;
        case GameObjectMetaType::StopSound: action = new StopSound(data, parent); break;
        case GameObjectMetaType::End: action = new End(parent); break;
        case GameObjectMetaType::GetUserInput: action = new GetUserInput(data, parent); break;
        case GameObjectMetaType::ChangeGameVariable: action = new ChangeGameVariable(data, parent); break;
        case GameObjectMetaType::RunScript: action = new RunScript(data, parent); break;
        default: break;
    }

    return action;
}

Action* GameObjectFactory::createAction(const QVariantMap & data, QObject * parent)
{
    GameObjectMetaType::Type type = GameObjectMetaType::typeFromString(data.value("type").toString());
    return createAction(type, data, parent);
}

Object* GameObjectFactory::createObject(GameObjectMetaType::Type type, const QVariantMap & data, QObject * parent)
{
    Object* obj = 0;

    switch(type) {
        case GameObjectMetaType::Object: obj = new Object(data, parent); break;
        case GameObjectMetaType::Image: obj = new Image(data, parent); break;
        case GameObjectMetaType::Character: obj = new Character(data, parent); break;
        case GameObjectMetaType::DialogueBox: obj = new DialogueBox(data, parent); break;
        case GameObjectMetaType::TextBox: obj = new TextBox(data, parent); break;
        case GameObjectMetaType::Button: obj = new Button(data, parent); break;
        case GameObjectMetaType::ObjectGroup: obj = new ObjectGroup(data, parent); break;
        case GameObjectMetaType::Menu: obj = new Menu(data, parent); break;
        case GameObjectMetaType::MenuOption: obj = new MenuOption(data, parent); break;
        default: break;
    }

    return obj;
}

Object* GameObjectFactory::createObject(const QVariantMap & data, QObject * parent)
{
    GameObjectMetaType::Type type = GameObjectMetaType::typeFromString(data.value("type").toString());
    return createObject(type, data, parent);
}

Scene* GameObjectFactory::createScene(const QVariantMap & data, QObject * parent)
{
    return new Scene(data, parent);
}



