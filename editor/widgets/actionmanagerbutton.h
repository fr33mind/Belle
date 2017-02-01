#ifndef ACTIONMANAGERBUTTON_H
#define ACTIONMANAGERBUTTON_H

#include <QPushButton>

#include "gameobjectmanager.h"

class GameObjectManager;

class ActionManagerButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ActionManagerButton(QWidget *parent = 0);

    GameObjectManager* actionManager() const;
    void setActionManager(GameObjectManager*);

    void hideAction(GameObjectMetaType::Type);
    void hideActions(const QList<GameObjectMetaType::Type>&);

    void showAction(GameObjectMetaType::Type);
    void showActions(const QList<GameObjectMetaType::Type>&);

public slots:
    void clear();

signals:
    void managingFinished();

private slots:
    void onClicked();
    void onActionManagerDestroyed();

private:
    GameObjectManager* mActionManager;
    QString mDefaultText;
    QList<GameObjectMetaType::Type> mActionFilter;

    void updateText();

};

#endif // ACTIONMANAGERBUTTON_H
