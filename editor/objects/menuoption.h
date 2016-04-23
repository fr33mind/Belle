#ifndef MENUOPTION_H
#define MENUOPTION_H

#include "button.h"

class Action;

class MenuOption : public Button
{
    Q_OBJECT

    QString mText;
    QList<Action*> mActions;
    QString mCondition;

public:
    MenuOption(const QString& text, const QList<Action*>& actions=QList<Action*>(), const QString& condition="", QObject* parent=0);
    MenuOption(const QVariantMap&, QObject* parent=0);
    virtual QVariantMap toJsonObject(bool internal=true) const;

    QList<Action*> actions() const;
    void setActions(const QList<Action*>&);
    void addAction(Action*);
    void removeActionAt(int);

    QString condition() const;
    void setCondition(const QString&);

private:
    void init();
};

#endif // MENUOPTION_H
