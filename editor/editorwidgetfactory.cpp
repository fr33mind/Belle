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

static QHash<EditorWidgetFactory::Type, GameObjectEditorWidget*> mEditorWidgets;
static QHash<EditorWidgetFactory::Type, QString> mEditorTypes;

void EditorWidgetFactory::load()
{
    mEditorTypes.insert(EditorWidgetFactory::Action, "Action");
    mEditorTypes.insert(EditorWidgetFactory::Show, "Show");
    mEditorTypes.insert(EditorWidgetFactory::Hide, "Hide");
    mEditorTypes.insert(EditorWidgetFactory::Wait, "Wait");
    mEditorTypes.insert(EditorWidgetFactory::Dialogue, "Dialogue");
    mEditorTypes.insert(EditorWidgetFactory::Slide, "Slide");
    mEditorTypes.insert(EditorWidgetFactory::Fade, "Fade");
    mEditorTypes.insert(EditorWidgetFactory::Label, "Label");
    mEditorTypes.insert(EditorWidgetFactory::GoToLabel, "GoToLabel");
    mEditorTypes.insert(EditorWidgetFactory::GoToScene, "GoToScene");
    mEditorTypes.insert(EditorWidgetFactory::Branch, "Branch");
    mEditorTypes.insert(EditorWidgetFactory::GetUserInput, "GetUserInput");
    mEditorTypes.insert(EditorWidgetFactory::ShowMenu, "ShowMenu");
    mEditorTypes.insert(EditorWidgetFactory::ChangeColor, "ChangeColor");
    mEditorTypes.insert(EditorWidgetFactory::PlaySound, "PlaySound");
    mEditorTypes.insert(EditorWidgetFactory::StopSound, "StopSound");
    mEditorTypes.insert(EditorWidgetFactory::ChangeGameVariable, "ChangeGameVariable");
    mEditorTypes.insert(EditorWidgetFactory::ChangeBackground, "ChangeBackground");
    mEditorTypes.insert(EditorWidgetFactory::RunScript, "RunScript");
    mEditorTypes.insert(EditorWidgetFactory::ChangeState, "ChangeState");
    mEditorTypes.insert(EditorWidgetFactory::End, "End");

    mEditorTypes.insert(EditorWidgetFactory::Object, "Object");
    mEditorTypes.insert(EditorWidgetFactory::TextBox, "TextBox");
    mEditorTypes.insert(EditorWidgetFactory::Button, "Button");
    mEditorTypes.insert(EditorWidgetFactory::Image, "Image");
    mEditorTypes.insert(EditorWidgetFactory::Character, "Character");
    mEditorTypes.insert(EditorWidgetFactory::ObjectGroup, "ObjectGroup");
    mEditorTypes.insert(EditorWidgetFactory::DialogueBox, "DialogueBox");
    mEditorTypes.insert(EditorWidgetFactory::Menu, "Menu");

    mEditorTypes.insert(EditorWidgetFactory::Scene, "Scene");

    QHashIterator<Type, QString> it(mEditorTypes);
    GameObjectEditorWidget* editor = 0;

    while(it.hasNext()) {
        it.next();

        editor = createEditorWidget(it.key());
        if (editor)
            mEditorWidgets.insert(it.key(), editor);
    }
}

GameObjectEditorWidget* EditorWidgetFactory::createEditorWidget(Type type)
{
    //actions
    if (type == Action)
        return new ActionEditorWidget;
    else if (type == Show)
        return new ShowEditorWidget;
    else if (type == Hide)
        return new HideEditorWidget;
    else if (type == Wait)
        return new WaitEditorWidget;
    else if (type == Dialogue)
        return new DialogueEditorWidget;
    else if (type == Slide)
        return new SlideEditorWidget;
    else if (type == Fade)
        return new FadeEditorWidget;
    else if (type == Label)
        return new LabelEditorWidget;
    else if (type == GoToLabel)
        return new GoToLabelEditorWidget;
    else if (type == GoToScene)
        return new GoToSceneEditorWidget;
    else if (type == Branch)
        return new BranchEditorWidget;
    else if (type == GetUserInput)
        return new GetUserInputEditorWidget;
    else if (type == ShowMenu)
        return new ShowMenuEditorWidget;
    else if (type == ChangeColor)
        return new ChangeColorEditorWidget;
    else if (type == PlaySound)
        return new PlaySoundEditorWidget;
    else if (type == StopSound)
        return new StopSoundEditorWidget;
    else if (type == ChangeGameVariable)
        return new ChangeGameVariableEditorWidget;
    else if (type == ChangeBackground)
        return new ChangeBackgroundEditorWidget;
    else if (type == RunScript)
        return new RunScriptEditorWidget;
    else if (type == ChangeState)
        return new ChangeStateEditorWidget;
    else if (type == End)
        return new GameObjectEditorWidget;

    //objects
    else if (type == Object)
        return new ObjectEditorWidget;
    else if (type == TextBox)
        return new TextPropertiesWidget;
    else if (type == Button)
        return new TextPropertiesWidget;
    else if (type == Image)
        return new ObjectEditorWidget;
    else if (type == Character)
        return new CharacterPropertiesWidget;
    else if (type == ObjectGroup)
        return new ObjectGroupEditorWidget;
    else if (type == DialogueBox)
        return new ObjectGroupEditorWidget;
    else if (type == Menu)
        return new ObjectGroupEditorWidget;

    //scene
    else if (type == Scene)
        return new SceneEditorWidget;

    return 0;
}

GameObjectEditorWidget* EditorWidgetFactory::createEditorWidget(const QString & type)
{
    createEditorWidget(typeFromString(type));
}

EditorWidgetFactory::Type EditorWidgetFactory::typeFromString(const QString& type)
{
    QHashIterator<Type, QString> it(mEditorTypes);
    QString ltype = type.toLower();

    while(it.hasNext()) {
        it.next();
        if (it.value().toLower() == ltype)
            return it.key();
    }

    return Unknown;
}

GameObjectEditorWidget* EditorWidgetFactory::editorWidget(Type type)
{
    return mEditorWidgets.value(type, 0);
}

GameObjectEditorWidget* EditorWidgetFactory::editorWidget(const QString& type)
{
    return editorWidget(typeFromString(type));
}

void EditorWidgetFactory::destroy()
{
    QHashIterator<Type, GameObjectEditorWidget*> it(mEditorWidgets);
    while(it.hasNext()) {
        it.next();
        if (it.value())
            it.value()->deleteLater();
    }
}
