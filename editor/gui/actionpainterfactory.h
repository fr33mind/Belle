#ifndef ACTIONPAINTERFACTORY_H
#define ACTIONPAINTERFACTORY_H

#include "gameobjectmetatype.h"
#include "actionpainter.h"

class ActionPainter;

class ActionPainterFactory
{
public:
    static void init();
    static void destroy();
    static ActionPainter* createActionPainter(GameObjectMetaType::Type);
    static ActionPainter* actionPainter(GameObjectMetaType::Type);

};

#endif // ACTIONPAINTERFACTORY_H
