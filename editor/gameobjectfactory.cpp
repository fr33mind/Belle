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
#include "changeobjectbackground.h"
#include "playsound.h"
#include "stopsound.h"
#include "show_menu.h"
#include "end.h"
#include "set_game_variable.h"
#include "change_background.h"
#include "runscript.h"
#include "changestate.h"
#include "sound.h"

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
    else if (type == GameObjectMetaType::Sound)
        obj = data.isEmpty() ? new Sound("", parent) : new Sound(data, parent);
    else if (type == GameObjectMetaType::Scene)
        obj = createScene(data, parent);

    return obj;
}

GameObject* GameObjectFactory::createGameObject(GameObjectMetaType::Type type, QObject * parent)
{
    return createGameObject(type, QVariantMap(), parent);
}

GameObject* GameObjectFactory::createGameObject(const QVariantMap & data, QObject * parent)
{
    GameObjectMetaType::Type type = GameObjectMetaType::typeFromString(data.value("type").toString());
    return createGameObject(type, data, parent);
}

Action* GameObjectFactory::createAction(GameObjectMetaType::Type type, const QVariantMap & data, QObject * parent)
{
    Action* action = 0;
    bool validData = !data.isEmpty();

    switch(type) {
        case GameObjectMetaType::Dialogue:
            action = validData ? new Dialogue(data, parent) : new Dialogue(parent);
            break;
        case GameObjectMetaType::Wait:
            action = validData ? new Wait(data, parent) : new Wait(parent);
            break;
        case GameObjectMetaType::Show:
            action = validData ? new Show(data, parent) : new Show(parent);
            break;
        case GameObjectMetaType::Hide:
            action = validData ? new Hide(data, parent) : new Hide(parent);
            break;
        case GameObjectMetaType::ChangeBackground:
            action = validData ? new ChangeBackground(data, parent) : new ChangeBackground(parent);
            break;
        case GameObjectMetaType::ChangeState:
            action = validData ? new ChangeState(data, parent) : new ChangeState(parent);
            break;
        case GameObjectMetaType::Label:
            action = validData ? new Label(data, parent) : new Label("", parent);
            break;
        case GameObjectMetaType::GoToLabel:
            action = validData ? new GoToLabel(data, parent) : new GoToLabel("", parent);
            break;
        case GameObjectMetaType::GoToScene:
            action = validData ? new GoToScene(data, parent) : new GoToScene(parent);
            break;
        case GameObjectMetaType::ShowMenu:
            action = validData ? new ShowMenu(data, parent) : new ShowMenu(parent);
            break;
        case GameObjectMetaType::Slide:
            action = validData ? new Slide(data, parent) : new Slide(parent);
            break;
        case GameObjectMetaType::Fade:
            action = validData ? new Fade(data, parent) : new Fade(parent);
            break;
        case GameObjectMetaType::Branch:
            action = validData ? new Branch(data, parent) : new Branch(parent);
            break;
        case GameObjectMetaType::ChangeObjectBackground:
            action = validData ? new ChangeObjectBackground(data, parent) : new ChangeObjectBackground(parent);
            break;
        case GameObjectMetaType::PlaySound:
            action = validData ? new PlaySound(data, parent) : new PlaySound(parent);
            break;
        case GameObjectMetaType::StopSound:
            action = validData ? new StopSound(data, parent) : new StopSound(parent);
            break;
        case GameObjectMetaType::End:
            action = validData ? new End(data, parent) : new End(parent);
            break;
        case GameObjectMetaType::GetUserInput:
            action = validData ? new GetUserInput(data, parent) : new GetUserInput(parent);
            break;
        case GameObjectMetaType::SetGameVariable:
            action = validData ? new SetGameVariable(data, parent) : new SetGameVariable(parent);
            break;
        case GameObjectMetaType::RunScript:
            action = validData ? new RunScript(data, parent) : new RunScript(parent);
            break;
        default: break;
    }

    return action;
}

Action* GameObjectFactory::createAction(const QVariantMap & data, QObject * parent)
{
    GameObjectMetaType::Type type = GameObjectMetaType::typeFromString(data.value("type").toString());
    return createAction(type, data, parent);
}

Action* GameObjectFactory::createAction(GameObjectMetaType::Type type, QObject * parent)
{
    return createAction(type, QVariantMap(), parent);
}

Object* GameObjectFactory::createObject(GameObjectMetaType::Type type, const QVariantMap & data, QObject * parent)
{
    Object* obj = 0;
    bool validData = !data.isEmpty();

    switch(type) {
        case GameObjectMetaType::Object:
            obj =  validData ? new Object(data, parent) : new Object(parent);
            break;
        case GameObjectMetaType::Image:
            obj = validData ? new Image(data, parent) : new Image("", parent);
            break;
        case GameObjectMetaType::Character:
            obj = validData ? new Character(data, parent) : new Character(parent);
            break;
        case GameObjectMetaType::DialogueBox:
            obj = validData ? new DialogueBox(data, parent) : new DialogueBox(parent);
            break;
        case GameObjectMetaType::TextBox:
            obj = validData ? new TextBox(data, parent) : new TextBox(parent);
            break;
        case GameObjectMetaType::Button:
            obj = validData ? new Button(data, parent) : new Button(parent);
            break;
        case GameObjectMetaType::ObjectGroup:
            obj = validData ? new ObjectGroup(data, parent) : new ObjectGroup(parent);
            break;
        case GameObjectMetaType::Menu:
            obj = validData ? new Menu(data, parent) : new Menu(parent);
            break;
        case GameObjectMetaType::MenuOption:
            obj = validData ? new MenuOption(data, parent) : new MenuOption("", parent);
            break;
        default: break;
    }

    return obj;
}

Object* GameObjectFactory::createObject(GameObjectMetaType::Type type, QObject * parent)
{
    return createObject(type, QVariantMap(), parent);
}

Object* GameObjectFactory::createObject(const QVariantMap & data, QObject * parent)
{
    GameObjectMetaType::Type type = GameObjectMetaType::typeFromString(data.value("type").toString());
    return createObject(type, data, parent);
}

Scene* GameObjectFactory::createScene(const QVariantMap & data, QObject * parent)
{
    bool validData = !data.isEmpty();
    Scene* scene = validData ? new Scene(data, parent) : new Scene(parent);
    return scene;
}



