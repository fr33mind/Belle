#ifndef ACTIONPAINTER_H
#define ACTIONPAINTER_H

#include <QObject>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>

#include "action.h"

#define BORDER 2
#define ITEM_SELECTED_LINE_WIDTH 4
#define MAX_ACTION_DISPLAY_LINES 3

class Action;

class ActionPainter : public QObject
{
    Q_OBJECT

public:
    explicit ActionPainter(Action* action = 0, QObject *parent = 0);
    virtual void paint (QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void setAction(Action*);
    Action* action() const;

protected:
    Action* mAction;

private:
    QString actionStatusText(const Action*) const;

signals:

private slots:
};

#endif // ACTIONPAINTER_H
