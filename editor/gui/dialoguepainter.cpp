#include "dialoguepainter.h"

DialoguePainter::DialoguePainter(Dialogue* dialogue, QObject* parent) :
    ActionPainter(dialogue, parent)
{

}

void DialoguePainter::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Action* action = mAction;
    if (!action)
        return;

    ActionPainter::paint(painter, option, index);

    Dialogue* dialogue = qobject_cast<Dialogue*>(action);
    if (!dialogue)
        return;

    if (dialogue->sound()) {
        QRect rect = option.rect;
        const QIcon playSoundIcon = GameObjectMetaType::icon(GameObjectMetaType::PlaySound);
        QString title = dialogue->title();
        QSize textSize = option.fontMetrics.size(0, title);
        int iconWidth = textSize.height();
        rect.setX(rect.width()-BORDER-iconWidth);
        rect.setY(rect.y()+BORDER);
        playSoundIcon.paint(painter, rect.x(), rect.y(), iconWidth, iconWidth);
    }
}
