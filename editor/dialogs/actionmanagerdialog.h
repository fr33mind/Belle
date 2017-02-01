#ifndef ACTIONMANAGERDIALOG_H
#define ACTIONMANAGERDIALOG_H

#include <QDialog>
#include "action_catalog_widget.h"
#include "actions_view.h"
#include "gameobject_editorwidget.h"

class ActionCatalogWidget;
class ActionsView;
class GameObjectEditorWidget;

class ActionManagerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ActionManagerDialog(GameObjectManager*, QWidget *parent = 0);
    virtual ~ActionManagerDialog();
    ActionCatalogWidget* actionCatalogWidget() const;

protected:
    void switchPropertiesEditorWidget(QWidget*);
    GameObjectEditorWidget* createPropertiesEditorWidget(GameObjectMetaType::Type);
    virtual void keyPressEvent(QKeyEvent *event);

signals:

private slots:
    void onNewAction(Action*);
    void onCurrentActionChanged(Action*);

private:
    QWidget* mPropertiesEditorParentWidget;
    ActionCatalogWidget* mActionCatalogWidget;
    ActionsView* mActionsView;
    GameObjectManager* mActionManager;
    Action* mCurrentAction;
    QHash<GameObjectMetaType::Type, GameObjectEditorWidget*> mPropertiesEditorWidgets;

};

#endif // ACTIONMANAGERDIALOG_H
