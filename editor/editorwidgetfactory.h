#ifndef EDITORWIDGETFACTORY_H
#define EDITORWIDGETFACTORY_H

#include <QHash>
#include <QString>

#include "gameobject_editorwidget.h"
#include "gameobjectmetatype.h"

class GameObjectEditorWidget;

class EditorWidgetFactory
{
public:
    static void init();
    static void destroy();
    static GameObjectEditorWidget* editorWidget(GameObjectMetaType::Type);
    static GameObjectEditorWidget* createEditorWidget(GameObjectMetaType::Type);
};

#endif // EDITORWIDGETFACTORY_H
