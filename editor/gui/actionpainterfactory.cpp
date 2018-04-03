#include "actionpainterfactory.h"

#include "dialoguepainter.h"

static QHash<GameObjectMetaType::Type, ActionPainter*> mActionPainters;

void ActionPainterFactory::init()
{
}

void ActionPainterFactory::destroy()
{
    QHashIterator<GameObjectMetaType::Type, ActionPainter*> it(mActionPainters);
    while(it.hasNext()) {
        it.next();
        if (it.value())
            it.value()->deleteLater();
    }
}

ActionPainter* ActionPainterFactory::createActionPainter(GameObjectMetaType::Type type)
{
    switch(type) {
        case GameObjectMetaType::Dialogue: return new DialoguePainter();
        default: break;
    }

    return new ActionPainter();
}

ActionPainter* ActionPainterFactory::actionPainter(GameObjectMetaType::Type type)
{
    ActionPainter* painter = mActionPainters.value(type, 0);
    if (! painter) {
        painter = createActionPainter(type);
        if (painter)
            mActionPainters.insert(type, painter);
    }
    return painter;
}
