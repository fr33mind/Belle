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
#include "change_game_variable_editor_widget.h"
#include "change_background_editorwidget.h"
#include "runscript_editorwidget.h"
#include "changestate_editorwidget.h"
#include "object_editor_widget.h"
#include "textbox_editor_widget.h"
#include "character_properties_widget.h"
#include "objectgroup_editor_widget.h"
#include "scene_editor_widget.h"

static QHash<QString, GameObjectEditorWidget*> mEditorWidgets;

void EditorWidgetFactory::load()
{
    //actions
    mEditorWidgets.insert("action", new ActionEditorWidget);
    mEditorWidgets.insert("show", new ShowEditorWidget);
    mEditorWidgets.insert("hide", new HideEditorWidget);
    mEditorWidgets.insert("wait", new WaitEditorWidget);
    mEditorWidgets.insert("dialogue", new DialogueEditorWidget);
    mEditorWidgets.insert("slide", new SlideEditorWidget);
    mEditorWidgets.insert("fade", new FadeEditorWidget);
    mEditorWidgets.insert("label", new LabelEditorWidget);
    mEditorWidgets.insert("gotolabel", new GoToLabelEditorWidget);
    mEditorWidgets.insert("gotoscene", new GoToSceneEditorWidget);
    mEditorWidgets.insert("branch", new BranchEditorWidget);
    mEditorWidgets.insert("getuserinput", new GetUserInputEditorWidget);
    mEditorWidgets.insert("showmenu", new ShowMenuEditorWidget);
    mEditorWidgets.insert("changecolor", new ChangeColorEditorWidget);
    mEditorWidgets.insert("playsound", new PlaySoundEditorWidget);
    mEditorWidgets.insert("stopsound", new StopSoundEditorWidget);
    mEditorWidgets.insert("changegamevariable", new ChangeGameVariableEditorWidget);
    mEditorWidgets.insert("changebackground", new ChangeBackgroundEditorWidget);
    mEditorWidgets.insert("runscript", new RunScriptEditorWidget);
    mEditorWidgets.insert("changestate", new ChangeStateEditorWidget);
    mEditorWidgets.insert("end", new GameObjectEditorWidget);

    //objects
    mEditorWidgets.insert("object", new ObjectEditorWidget);
    mEditorWidgets.insert("textbox", new TextPropertiesWidget);
    mEditorWidgets.insert("button", new TextPropertiesWidget);
    mEditorWidgets.insert("image", new ObjectEditorWidget);
    mEditorWidgets.insert("character", new CharacterPropertiesWidget);
    mEditorWidgets.insert("objectgroup", new ObjectGroupEditorWidget);
    mEditorWidgets.insert("dialoguebox", new ObjectGroupEditorWidget);
    mEditorWidgets.insert("menu", new ObjectGroupEditorWidget);

    //scene
    mEditorWidgets.insert("scene", new SceneEditorWidget);
}

GameObjectEditorWidget* EditorWidgetFactory::editorWidget(const QString& type)
{
    return mEditorWidgets.value(type.toLower(), 0);
}

void EditorWidgetFactory::destroy()
{
    QHashIterator<QString, GameObjectEditorWidget*> it(mEditorWidgets);
    while(it.hasNext()) {
        it.next();
        if (it.value())
            it.value()->deleteLater();
    }
}
