#ifndef EDITORWIDGETFACTORY_H
#define EDITORWIDGETFACTORY_H

#include <QHash>
#include <QString>

#include "gameobject_editorwidget.h"

class GameObjectEditorWidget;

class EditorWidgetFactory
{
public:
    enum Type {
        Unknown=0,
        Action,
        Show,
        Hide,
        Wait,
        Dialogue,
        Slide,
        Fade,
        Label,
        GoToLabel,
        GoToScene,
        Branch,
        GetUserInput,
        ShowMenu,
        ChangeColor,
        PlaySound,
        StopSound,
        ChangeGameVariable,
        ChangeBackground,
        RunScript,
        ChangeState,
        End,
        Object,
        TextBox,
        Button,
        Image,
        Character,
        ObjectGroup,
        DialogueBox,
        Menu,
        Scene
    };

    static void load();
    static void destroy();
    static GameObjectEditorWidget* editorWidget(Type);
    static GameObjectEditorWidget* editorWidget(const QString&);
    static GameObjectEditorWidget* createEditorWidget(Type);
    static GameObjectEditorWidget* createEditorWidget(const QString&);
    static Type typeFromString(const QString&);
};

#endif // EDITORWIDGETFACTORY_H
