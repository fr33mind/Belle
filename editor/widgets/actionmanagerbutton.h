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

    void updateText();

};

#endif // ACTIONMANAGERBUTTON_H
