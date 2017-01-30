#ifndef MENUOPTION_H
#define MENUOPTION_H

#include "button.h"
#include "gameobjectmanager.h"

class Action;
class GameObjectManager;

class MenuOption : public Button
{
    Q_OBJECT

    QString mText;
    GameObjectManager* mActionManager;
    QString mCondition;

public:
    explicit MenuOption(const QString& text="", QObject* parent=0);
    MenuOption(const QString& text, const QList<Action*>& actions, const QString& condition="", QObject* parent=0);
    MenuOption(const QVariantMap&, QObject* parent=0);
    virtual QVariantMap toJsonObject(bool internal=true) const;

    QList<Action*> actions() const;
    GameObjectManager* actionManager() const;
    void setActions(const QList<Action*>&);
    void addAction(Action*);
    void removeActionAt(int);

    QString condition() const;
    void setCondition(const QString&);

protected:
    virtual void loadData(const QVariantMap&, bool internal=false);

private slots:
    void onActionManagerDestroyed();

private:
    void init();
    GameObjectManager* createActionManager();
};

#endif // MENUOPTION_H
