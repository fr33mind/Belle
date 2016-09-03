#include "editorwidgetfactory.h"

#include <QHashIterator>

#include "action_editor_widget.h"
#include "show_editor_widget.h"
#include "hide_editor_widget.h"
#include "wait_editor_widget.h"
#include "dialogue_editor_widget.h"
#include "slide_editor_widget.h"
#include "fade_editor_widget.h"
#include "label_editor_widget.h"
#include "gotolabel_editor_widget.h"
#include "gotoscene_editorwidget.h"
#include "branch_editor_widget.h"
#include "get_user_input_editor_widget.h"
#include "show_menu_editorwidget.h"
#include "changecolor_editor_widget.h"
#include "playsound_editorwidget.h"
#include "stopsound_editorwidget.h"
#include "set_game_variable_editor_widget.h"
#include "change_background_editorwidget.h"
#include "runscript_editorwidget.h"
#include "changestate_editorwidget.h"
#include "object_editor_widget.h"
#include "textbox_editor_widget.h"
#include "character_properties_widget.h"
#include "objectgroup_editor_widget.h"
#include "scene_editor_widget.h"
#include "soundeditorwidget.h"

static QHash<GameObjectMetaType::Type, GameObjectEditorWidget*> mEditorWidgets;

void EditorWidgetFactory::init()
{
}

GameObjectEditorWidget* EditorWidgetFactory::createEditorWidget(GameObjectMetaType::Type type)
{
    if (type == GameObjectMetaType::GameObject)
        return new GameObjectEditorWidget;
    //actions
    else if (type == GameObjectMetaType::Action)
        return new ActionEditorWidget;
    else if (type == GameObjectMetaType::Show)
        return new ShowEditorWidget;
    else if (type == GameObjectMetaType::Hide)
        return new HideEditorWidget;
    else if (type == GameObjectMetaType::Wait)
        return new WaitEditorWidget;
    else if (type == GameObjectMetaType::Dialogue)
        return new DialogueEditorWidget;
    else if (type == GameObjectMetaType::Slide)
        return new SlideEditorWidget;
    else if (type == GameObjectMetaType::Fade)
        return new FadeEditorWidget;
    else if (type == GameObjectMetaType::Label)
        return new LabelEditorWidget;
    else if (type == GameObjectMetaType::GoToLabel)
        return new GoToLabelEditorWidget;
    else if (type == GameObjectMetaType::GoToScene)
        return new GoToSceneEditorWidget;
    else if (type == GameObjectMetaType::Branch)
        return new BranchEditorWidget;
    else if (type == GameObjectMetaType::GetUserInput)
        return new GetUserInputEditorWidget;
    else if (type == GameObjectMetaType::ShowMenu)
        return new ShowMenuEditorWidget;
    else if (type == GameObjectMetaType::ChangeColor)
        return new ChangeColorEditorWidget;
    else if (type == GameObjectMetaType::PlaySound)
        return new PlaySoundEditorWidget;
    else if (type == GameObjectMetaType::StopSound)
        return new StopSoundEditorWidget;
    else if (type == GameObjectMetaType::SetGameVariable)
        return new SetGameVariableEditorWidget;
    else if (type == GameObjectMetaType::ChangeBackground)
        return new ChangeBackgroundEditorWidget;
    else if (type == GameObjectMetaType::RunScript)
        return new RunScriptEditorWidget;
    else if (type == GameObjectMetaType::ChangeState)
        return new ChangeStateEditorWidget;
    else if (type == GameObjectMetaType::End)
        return new GameObjectEditorWidget;
    //objects
    else if (type == GameObjectMetaType::Object)
        return new ObjectEditorWidget;
    else if (type == GameObjectMetaType::TextBox)
        return new TextPropertiesWidget;
    else if (type == GameObjectMetaType::Button)
        return new TextPropertiesWidget;
    else if (type == GameObjectMetaType::Image)
        return new ObjectEditorWidget;
    else if (type == GameObjectMetaType::Character)
        return new CharacterPropertiesWidget;
    else if (type == GameObjectMetaType::ObjectGroup)
        return new ObjectGroupEditorWidget;
    else if (type == GameObjectMetaType::DialogueBox)
        return new ObjectGroupEditorWidget;
    else if (type == GameObjectMetaType::Menu)
        return new ObjectGroupEditorWidget;
    else if (type == GameObjectMetaType::Sound)
        return new SoundEditorWidget;
    //scene
    else if (type == GameObjectMetaType::Scene)
        return new SceneEditorWidget;

    return 0;
}

GameObjectEditorWidget* EditorWidgetFactory::editorWidget(GameObjectMetaType::Type type)
{
    GameObjectEditorWidget* widget = mEditorWidgets.value(type, 0);
    if (! widget) {
        widget = createEditorWidget(type);
        if (widget)
            mEditorWidgets.insert(type, widget);
    }
    return widget;
}

void EditorWidgetFactory::destroy()
{
    QHashIterator<GameObjectMetaType::Type, GameObjectEditorWidget*> it(mEditorWidgets);
    while(it.hasNext()) {
        it.next();
        if (it.value())
            it.value()->deleteLater();
    }
}
