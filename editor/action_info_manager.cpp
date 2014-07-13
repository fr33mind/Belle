/* Copyright (C) 2012-2014 Carlos Pais
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "action_info_manager.h"

#include <QDebug>

#include "show.h"
#include "hide.h"
#include "dialogue.h"
#include "label.h"
#include "gotolabel.h"
#include "gotoscene.h"
#include "fade.h"
#include "slide.h"
#include "wait.h"
#include "branch.h"
#include "get_user_input.h"
#include "show_menu.h"
#include "changecolor.h"
#include "playsound.h"
#include "stopsound.h"
#include "change_game_variable.h"
#include "change_background.h"
#include "end.h"
#include "runscript.h"

QList<Action*> mActionsCatalog = QList<Action*>();
QHash<QString, QIcon> mTypeToIcon;

ActionInfoManager::ActionInfoManager(QObject *parent) :
    QObject(parent)
{
}

void ActionInfoManager::initEditorWidgets()
{
    Action::setActionEditorWidget(new ActionEditorWidget);
    Show::setShowEditorWidget(new ShowEditorWidget());
    Hide::setHideEditorWidget(new HideEditorWidget());
    //ChangeVisibility::setChangeVisibilityEditorWidget(new ChangeVisibilityEditorWidget(ResourceManager::instance()));
    Wait::setWaitEditorWidget(new WaitEditorWidget());
    Dialogue::setDialogueEditorWidget(new DialogueEditorWidget());
    Slide::setSlideEditorWidget(new SlideEditorWidget());
    Fade::setFadeEditorWidget(new FadeEditorWidget());
    Label::setLabelEditorWidget(new LabelEditorWidget());
    GoToLabel::setGoToLabelEditorWidget(new GoToLabelEditorWidget());
    GoToScene::setGoToSceneEditorWidget(new GoToSceneEditorWidget());
    Branch::setBranchEditorWidget(new BranchEditorWidget());
    GetUserInput::setGetUserInputEditorWidget(new GetUserInputEditorWidget);
    ShowMenu::setShowMenuEditorWidget(new ShowMenuEditorWidget());
    ChangeColor::setChangeColorEditorWidget(new ChangeColorEditorWidget);
    PlaySound::setPlaySoundEditorWidget(new PlaySoundEditorWidget());
    StopSound::setStopSoundEditorWidget(new StopSoundEditorWidget());
    ChangeGameVariable::setChangeGameVariableEditorWidget(new ChangeGameVariableEditorWidget);
    ChangeBackground::setChangeBackgroundEditorWidget(new ChangeBackgroundEditorWidget);
    RunScript::setRunScriptEditorWidget(new RunScriptEditorWidget);
}

void ActionInfoManager::init()
{
    /*ActionInfo* info = new ActionInfo();
    info->name = "Action";
    info->type = "Action";
    Action::setActionInfo(info);
    info = new ActionInfo();
    info->name = "Branch";
    info->type = "Branch";
    Branch::setBranchActionInfo(info);*/


    Action::Info = ActionInfo(QIcon(), "Action", tr("Action"));
    Show::Info = ActionInfo(QIcon(":/media/show.png"), "Show", tr("Show"));
    Hide::Info = ActionInfo(QIcon(":/media/hide.png"), "Hide", tr("Hide"));
    Wait::Info = ActionInfo(QIcon(":/media/timer.png"), "Wait", tr("Wait"));
    Dialogue::Info = ActionInfo(QIcon(":/media/talk-baloon.png"), "Dialogue", tr("Dialogue"));
    Slide::Info = ActionInfo(QIcon(":/media/slide.png"), "Slide", tr("Slide"));
    Fade::Info = ActionInfo(QIcon(":/media/fade.png"), "Fade", tr("Fade"));
    Label::Info = ActionInfo(QIcon(":/media/label.png"), "Label", tr("Label"));
    GoToLabel::Info = ActionInfo(QIcon(":/media/go-to-label.png"), "GoToLabel", tr("Go To Label"));
    GoToScene::Info = ActionInfo(QIcon(":/media/go-to-scene.png"), "GoToScene", tr("Go To Scene"));
    Branch::Info = ActionInfo(QIcon(":/media/branch.png"), "Branch", tr("Branch"));
    GetUserInput::Info = ActionInfo(QIcon(":/media/edit-clear.png"), "GetUserInput", tr("Get User Input"));
    ShowMenu::Info = ActionInfo(QIcon(":/media/menu.png"), "ShowMenu", tr("Show Menu"));
    ChangeColor::Info = ActionInfo(QIcon(":/media/color.png"), "ChangeColor", tr("Change Color"));
    PlaySound::Info = ActionInfo(QIcon(":/media/sound.png"), "PlaySound", tr("Play Sound"));
    StopSound::Info = ActionInfo(QIcon(":/media/no-sound.png"), "StopSound", tr("Stop Sound"));
    ChangeGameVariable::Info = ActionInfo(QIcon(":/media/script.png"), "ChangeGameVariable", tr("Change Game Variable"));
    ChangeBackground::Info = ActionInfo(QIcon(":/media/image.png"), "ChangeBackground", tr("Change Background"));
    End::Info = ActionInfo(QIcon(":/media/end-novel.png"), "EndNovel", tr("End"));
    RunScript::Info = ActionInfo(QIcon(":/media/script.png"), "RunScript", tr("Run Script"));

    mTypeToIcon.insert("Action", QIcon());
    mTypeToIcon.insert("Branch", QIcon(":/media/branch.png"));
    mTypeToIcon.insert("Show", QIcon(":/media/show.png"));
    mTypeToIcon.insert("Hide", QIcon(":/media/hide.png"));
    mTypeToIcon.insert("Wait", QIcon(":/media/timer.png"));
    mTypeToIcon.insert("Label", QIcon(":/media/label.png"));
    mTypeToIcon.insert("Dialogue", QIcon(":/media/talk-baloon.png"));
    mTypeToIcon.insert("Fade", QIcon(":/media/fade.png"));
    mTypeToIcon.insert("Slide", QIcon(":/media/slide.png"));
    mTypeToIcon.insert("ChangeColor", QIcon(":/media/color.png"));
    mTypeToIcon.insert("GoToLabel", QIcon(":/media/go-to-label.png"));
    mTypeToIcon.insert("GoToScene", QIcon(":/media/go-to-scene.png"));
    mTypeToIcon.insert("GetUserInput", QIcon(":/media/edit-clear.png"));
    mTypeToIcon.insert("ShowMenu", QIcon(":/media/menu.png"));
    mTypeToIcon.insert("PlaySound", QIcon(":/media/sound.png"));
    mTypeToIcon.insert("StopSound", QIcon(":/media/no-sound.png"));
    mTypeToIcon.insert("RunScript", QIcon(":/media/script.png"));

    initEditorWidgets();

    registerActions();
}

void ActionInfoManager::registerAction(const QIcon& icon, const QString& name, const QString& description, Interaction::InputEvents events, Action* action)
{
    action->setIcon(icon);
    action->setObjectName(name);
    action->setDescription(description);
    action->setSupportedEvents(events);

    mActionsCatalog.append(action);
}

void ActionInfoManager::registerActions()
{
}


QList<Action*> ActionInfoManager::actions()
{
    return mActionsCatalog;
}

Action* ActionInfoManager::typeToAction(const QVariant& data, QObject* parent)
{
    if (data.type() != QVariant::Map)
        return 0;

    QVariantMap actionMap = data.toMap();

    if (! actionMap.contains("type") || actionMap.value("type").type() != QVariant::String)
        return 0;

    QString type = actionMap.value("type").toString();


    /*for(int i=0; i < mActionsCatalog.size(); i++) {
        if (mActionsCatalog[i]->type() == type)
            return mActionsCatalog[i]->copy();
    }*/

    if (type == "Action")
        return new Action(actionMap, parent);
    else if (type == "Show")
        return new Show(actionMap, parent);
    else if (type == "Hide")
        return new Hide(actionMap, parent);
    else if (type == "Dialogue")
        return new Dialogue(actionMap, parent);
    else if (type == "Branch")
        return new Branch(actionMap, parent);
    else if (type == "ChangeBackground")
        return new ChangeBackground(actionMap, parent);
    else if (type == "Fade")
        return new Fade(actionMap, parent);
    else if (type == "Label")
        return new Label(actionMap, parent);
    else if (type == "GoToLabel")
        return new GoToLabel(actionMap, parent);
    else if (type == "GoToScene")
        return new GoToScene(actionMap, parent);
    else if (type == "Wait")
        return new Wait(actionMap, parent);
    else if (type == "GetUserInput")
        return new GetUserInput(actionMap, parent);
    else if (type == "ShowMenu")
        return new ShowMenu(actionMap, parent);
    else if (type == "ChangeColor")
        return new ChangeColor(actionMap, parent);
    else if (type == "Slide")
        return new Slide(actionMap, parent);
    else if (type == "PlaySound")
        return new PlaySound(actionMap, parent);
    else if (type == "StopSound")
        return new StopSound(actionMap, parent);
    else if (type == "ChangeGameVariable")
        return new ChangeGameVariable(actionMap, parent);
    else if (type == "RunScript")
        return new RunScript(actionMap, parent);

    return 0;
}

void ActionInfoManager::destroy()
{
    if (Action::actionEditorWidget())
        Action::actionEditorWidget()->deleteLater();

    if (Show::showEditorWidget())
        Show::showEditorWidget()->deleteLater();

    if (Hide::hideEditorWidget())
        Hide::hideEditorWidget()->deleteLater();

    if (Dialogue::dialogueEditorWidget())
        Dialogue::dialogueEditorWidget()->deleteLater();

    if (Slide::slideEditorWidget())
        Slide::slideEditorWidget()->deleteLater();

    if (Fade::fadeEditorWidget())
        Fade::fadeEditorWidget()->deleteLater();

    if (Label::labelEditorWidget())
        Label::labelEditorWidget()->deleteLater();

    if (GoToLabel::goToLabelEditorWidget())
        GoToLabel::goToLabelEditorWidget()->deleteLater();

    if (GoToScene::goToSceneEditorWidget())
        GoToScene::goToSceneEditorWidget()->deleteLater();

    if (Branch::branchEditorWidget())
        Branch::branchEditorWidget()->deleteLater();

    if (GetUserInput::getUserInputEditorWidget())
        GetUserInput::getUserInputEditorWidget()->deleteLater();

    if (ShowMenu::showMenuEditorWidget())
        ShowMenu::showMenuEditorWidget()->deleteLater();

    if (ChangeColor::changeColorEditorWidget())
        ChangeColor::changeColorEditorWidget()->deleteLater();

    if (Wait::waitEditorWidget())
        Wait::waitEditorWidget()->deleteLater();

    if (PlaySound::playSoundEditorWidget())
        PlaySound::playSoundEditorWidget()->deleteLater();

    if (StopSound::stopSoundEditorWidget())
        StopSound::stopSoundEditorWidget()->deleteLater();

    if (ChangeGameVariable::changeGameVariableEditorWidget())
        ChangeGameVariable::changeGameVariableEditorWidget()->deleteLater();

    if (ChangeBackground::changeBackgroundEditorWidget())
        ChangeBackground::changeBackgroundEditorWidget()->deleteLater();

    if (RunScript::runScriptEditorWidget())
        RunScript::runScriptEditorWidget()->deleteLater();

    for(int i=0; i < mActionsCatalog.size(); i++)
        mActionsCatalog[i]->deleteLater();
    mActionsCatalog.clear();
}
