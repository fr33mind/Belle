#include "actionpainter.h"

#include "actionmetatype.h"

ActionPainter::ActionPainter(Action* action, QObject *parent) :
    QObject(parent)
{
    mAction = 0;
    setAction(action);
}

void ActionPainter::setAction(Action * action)
{
    mAction = action;
}

Action* ActionPainter::action() const
{
    return mAction;
}

void ActionPainter::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    Action* action = mAction;
    if (!action)
        return;

    QRect rect = option.rect;
    QRect textRect = rect;
    textRect.setX(BORDER);
    textRect.setY(textRect.y()+BORDER);
    QColor textColor(83, 83, 83);

    painter->fillRect(rect, ActionMetaType::color(action));

    if (option.state.testFlag(QStyle::State_Selected)) {
        painter->fillRect(rect, option.palette.brush(QPalette::Highlight));
        textColor = option.palette.highlightedText().color();
    }

    if (option.state.testFlag(QStyle::State_MouseOver)) {
        //TODO: Show light selection color when dragging.
    }

    painter->save();
    QPen pen = painter->pen();
    pen.setBrush(option.palette.base());
    painter->setPen(pen);
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());
    painter->restore();

    const GameObjectMetaType* metatype = GameObjectMetaType::metaType(action->type());
    const QIcon typeIcon = metatype ? metatype->icon() : QIcon();
    QString title = action->title();
    QString actionStatusText = this->actionStatusText(action);
    QSize textSize = option.fontMetrics.size(0, title);
    int textHeight = textSize.height();
    typeIcon.paint(painter, textRect.x(), textRect.y(), textHeight, textHeight);

    textRect.setX(textHeight+BORDER*2);
    pen = painter->pen();
    pen.setColor(textColor);
    painter->setPen(pen);
    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(textRect, title);
    font.setBold(false);
    painter->setFont(font);

    if (!actionStatusText.isEmpty()) {
        QRect textRect2 = textRect;
        textRect2.setX(textRect.x() + textSize.width());
        painter->drawText(textRect2, actionStatusText);
    }

    QString displayText = action->displayText();

    if (! displayText.isEmpty()) {
        textRect.setY(textRect.y() + textHeight);
        QStringList lines = displayText.split("\n");
        QString line;
        int lineWidth = 0;
        int marginWidth = option.fontMetrics.width("...") + 2;

        if (lines.size() > MAX_ACTION_DISPLAY_LINES) {
            lines = lines.mid(0, MAX_ACTION_DISPLAY_LINES);
            lines.append("...");
        }

        for(int i=0; i < lines.size(); i++) {
            line = lines.at(i);
            lineWidth = option.fontMetrics.width(line);

            if (lineWidth > textRect.width()) {
                for(int j=line.size()-1; j >= 0 && option.fontMetrics.width(line) >= textRect.width() - marginWidth; j--) {
                    line.remove(j, 1);
                }
                line.append("...");
            }

            lines[i] = line;
        }

        painter->drawText(textRect, Qt::TextWordWrap, lines.join("\n"), &textRect);
    }
}

QSize ActionPainter::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Action* action = mAction;
    QSize size = option.rect.size();

    if (!action)
        return size;

    int height = 1; //start at 1 for the bottom border

    height += option.fontMetrics.size(0, action->name()).height() + BORDER*2;

    QString displayText = action->displayText();
    if (! displayText.isEmpty()) {
        QStringList lines = displayText.split("\n");

        if (lines.size() > MAX_ACTION_DISPLAY_LINES) {
            lines = lines.mid(0, MAX_ACTION_DISPLAY_LINES);
            lines.append("...");
        }

        foreach(const QString& line, lines) {
            height += option.fontMetrics.size(Qt::TextSingleLine, line).height();
        }

        height += BORDER;
    }

    size.setHeight(height);

    return size;
}

QString ActionPainter::actionStatusText(const Action * action) const
{
    QString text;

    if (!action)
        return text;

    QString syncedText = tr("Synced");
    QString notSyncedText = tr("Not Synced");
    QString syncStatusText = "";
    GameObject* actionResource = action->resource();

    if (actionResource) {
        if (action->isSynced())
            syncStatusText = syncedText;
        else
            syncStatusText = notSyncedText;

        syncStatusText = QString(" (%1)").arg(syncStatusText);
    }

    text = syncStatusText;

    return text;
}
