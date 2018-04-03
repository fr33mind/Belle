#ifndef DIALOGUEPAINTER_H
#define DIALOGUEPAINTER_H

#include "actionpainter.h"
#include "dialogue.h"

class Dialogue;

class DialoguePainter : public ActionPainter
{
    Q_OBJECT

public:
    DialoguePainter(Dialogue* dialogue=0, QObject* parent=0);
    virtual void paint (QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

#endif // DIALOGUEPAINTER_H
