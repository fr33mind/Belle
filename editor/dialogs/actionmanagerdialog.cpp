#include "actionmanagerdialog.h"

#include <QVBoxLayout>

#include "editorwidgetfactory.h"

ActionManagerDialog::ActionManagerDialog(GameObjectManager* actionManager, QWidget *parent) :
    QDialog(parent)
{
    mCurrentAction = 0;
    mActionManager = actionManager;
    mActionsView = new ActionsView(this);
    mActionsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ActionsModel* model = qobject_cast<ActionsModel*>(mActionsView->model());
    if (model)
        model->setActionManager(mActionManager);
    mActionCatalogWidget = new ActionCatalogWidget(this);
    mActionCatalogWidget->setMaximumHeight(200);
    mPropertiesEditorParentWidget = new QWidget(this);

    if (mPropertiesEditorParentWidget->layout())
        delete mPropertiesEditorParentWidget->layout();
    mPropertiesEditorParentWidget->setLayout(new QVBoxLayout());

    QVBoxLayout* vlayout = new QVBoxLayout(this);
    vlayout->addWidget(mActionsView);
    vlayout->addWidget(mActionCatalogWidget);
    vlayout->addWidget(mPropertiesEditorParentWidget);
    mPropertiesEditorParentWidget->hide();

    connect(mActionCatalogWidget, SIGNAL(newAction(Action*)), this, SLOT(onNewAction(Action*)));
    connect(mActionsView, SIGNAL(currentActionChanged(Action*)), this, SLOT(onCurrentActionChanged(Action*)));

    setWindowTitle(tr("Edit Actions"));
    resize(640, height());
}

ActionManagerDialog::~ActionManagerDialog()
{
    if (mCurrentAction)
        mCurrentAction->focusOut();
}

void ActionManagerDialog::onCurrentActionChanged(Action * action)
{
    if (mCurrentAction)
        mCurrentAction->focusOut();
    mCurrentAction = action;
    if (mCurrentAction)
        mCurrentAction->focusIn();

    if (action) {
        GameObjectEditorWidget* editor = createPropertiesEditorWidget(action->type());
        switchPropertiesEditorWidget(editor);
        if (editor)
            editor->setGameObject(action);
        mActionCatalogWidget->hide();
        mPropertiesEditorParentWidget->show();
    }
    else {
        mActionCatalogWidget->show();
        mPropertiesEditorParentWidget->hide();
    }
}

GameObjectEditorWidget* ActionManagerDialog::createPropertiesEditorWidget(GameObjectMetaType::Type type)
{
    if (mPropertiesEditorWidgets.contains(type))
        return mPropertiesEditorWidgets.value(type);

    GameObjectEditorWidget* editor = EditorWidgetFactory::createEditorWidget(type);
    if (editor) {
        editor->setParent(mPropertiesEditorParentWidget);
        mPropertiesEditorWidgets.insert(type, editor);
    }

    return editor;
}

void ActionManagerDialog::onNewAction(Action * action)
{
    mActionsView->addActionItem(action);
}

void ActionManagerDialog::switchPropertiesEditorWidget(QWidget* widget)
{
    QLayout *layout = mPropertiesEditorParentWidget->layout();
    if (! layout)
        return;

    QLayoutItem* item = layout->takeAt(0);
    if (item && item->widget())
        item->widget()->hide();

    if (widget) {
        layout->addWidget(widget);
        if (widget->isHidden())
            widget->show();
    }
}

void ActionManagerDialog::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        if (mCurrentAction) {
            mActionsView->selectionModel()->clear();
            return;
        }
    }

    QDialog::keyPressEvent(event);
}
