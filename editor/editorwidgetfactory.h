#ifndef EDITORWIDGETFACTORY_H
#define EDITORWIDGETFACTORY_H

#include <QHash>
#include <QString>

#include "gameobject_editorwidget.h"

class GameObjectEditorWidget;

class EditorWidgetFactory
{
public:
    static void load();
    static void destroy();
    static GameObjectEditorWidget* editorWidget(const QString&);
};

#endif // EDITORWIDGETFACTORY_H
