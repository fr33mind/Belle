/* Copyright (C) 2012-2014 Carlos Pais
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "belle.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QCheckBox>
#include <QHash>
#include <QString>
#include <QFileDialog>
#include <QString>
#include <QUrl>
#include <QFile>
#include <QDesktopServices>
#include <QMessageBox>
#include <QTextCodec>
#include <QProcess>
#include <QJsonDocument>

#include "object.h"
#include "add_character_dialog.h"
#include "dialogue.h"
#include "properties_widget.h"
#include "textbox_editor_widget.h"
#include "character.h"
#include "action_catalog_widget.h"
#include "actions_model.h"
#include "change_visibility.h"
#include "wait.h"
#include "image.h"
#include "button.h"
#include "resources_view.h"
#include "condition_dialog.h"
#include "label.h"
#include "aboutdialog.h"
#include "slide.h"
#include "novel_properties_dialog.h"
#include "utils.h"
#include "engine.h"
#include "simple_http_server.h"
#include "save_project_dialog.h"
#include "update_elements_dialog.h"
#include "objectsview.h"
#include "editorwidgetfactory.h"

static Belle* mInstance = 0;

Belle::Belle(QWidget *widget)
    : QMainWindow(widget)
{
    mUi.setupUi( this );

    mInstance = this;
    mClipboard = new Clipboard(this);

    //init webview
    mWebView = new QWebView(this);
    mWebView->setWindowFlags(mWebView->windowFlags() | Qt::Window);
    mWebView->setWindowModality(Qt::WindowModal);
    QWebSettings* webSettings = mWebView->settings();
    webSettings->setAttribute(QWebSettings::LocalStorageEnabled, true);

    mHttpServer.setServerPort(8000);
    mDisableClick = false;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    //init scene manager instance
    mDefaultSceneManager = new SceneManager(WIDTH, HEIGHT, this, "DefaultSceneManager");
    mPauseSceneManager = new SceneManager(WIDTH, HEIGHT, this, "PauseSceneManager");
    mCurrentSceneManager = mDefaultSceneManager;
    //connect(SceneManager::instance(), SIGNAL(selectionChanged(Object*)), this, SLOT(onSelectedObjectChanged(Object*)));
    connect(mDefaultSceneManager, SIGNAL(sceneRemoved(int)), this, SLOT(onSceneRemoved(int)));
    connect(mPauseSceneManager, SIGNAL(sceneRemoved(int)), this, SLOT(onSceneRemoved(int)));
    connect(mDefaultSceneManager, SIGNAL(sceneNameChanged(int, const QString&)), this, SLOT(onSceneNameChanged(int, const QString&)));
    connect(mPauseSceneManager, SIGNAL(sceneNameChanged(int, const QString&)), this, SLOT(onSceneNameChanged(int, const QString&)));
    mDefaultSceneManager->setClipboard(mClipboard);
    mPauseSceneManager->setClipboard(mClipboard);

    //init drawing widget
    QLayout *vLayout = centralWidget()->layout();
    QScrollArea * scrollArea = new QScrollArea(mUi.centralwidget);
    mDrawingSurfaceWidget = new DrawingSurfaceWidget(mDefaultSceneManager, scrollArea);
    connect(mDrawingSurfaceWidget, SIGNAL(selectionChanged(Object*)), this, SLOT(onSelectedObjectChanged(Object*)));
    scrollArea->setWidget(mDrawingSurfaceWidget);
    scrollArea->setContentsMargins(0, 0, 0, 0);
    scrollArea->viewport()->installEventFilter(mDrawingSurfaceWidget);
    scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    vLayout->addWidget(scrollArea);

    //setup default data
    QVariantMap data;
    data.insert("title", tr("Untitled"));
    data.insert("width", WIDTH);
    data.insert("height", HEIGHT);
    data.insert("textSpeed", 50);
    data.insert("fontSize", 18);
    data.insert("fontFamily", QFontInfo(QFont()).family());
    setNovelProperties(data);

    mUi.scenesWidget->setIconSize(QSize(64, 48));
    mUi.pauseScenesWidget->setIconSize(QSize(64, 48));

    //create editors
    EditorWidgetFactory::load();
    ActionInfoManager::init();

    mActionsView = new ActionsView(this);
    ActionsModel * actionsModel = qobject_cast<ActionsModel*> (mActionsView->model());
    QLayout* layout = mUi.actionsViewWidget->layout();

    if (layout)
        layout->addWidget(mActionsView);

    //connect(mDrawingSurfaceWidget, SIGNAL(paintFinished()), this, SLOT(updateSceneIcon()));
    connect(mActionsView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onActionsViewClicked(const QModelIndex&)));

    //menu bar connections
    connect(mUi.propertiesAction, SIGNAL(triggered()), this, SLOT(onPropertiesTriggered()));

    //add available resource items
    mUi.twObjects->addTopLevelItem(new QTreeWidgetItem(mUi.twObjects, QStringList()<< tr("Character")));
    mUi.twObjects->addTopLevelItem(new QTreeWidgetItem(mUi.twObjects, QStringList()<< tr("Text Box")));
    mUi.twObjects->addTopLevelItem(new QTreeWidgetItem(mUi.twObjects, QStringList()<< tr("Image")));
    mUi.twObjects->addTopLevelItem(new QTreeWidgetItem(mUi.twObjects, QStringList()<< tr("Dialogue Box")));
    mUi.twObjects->addTopLevelItem(new QTreeWidgetItem(mUi.twObjects, QStringList()<< tr("Button")));

    mUi.twObjects->topLevelItem(0)->setIcon(0, QIcon(":/media/user.png"));
    mUi.twObjects->topLevelItem(1)->setIcon(0, QIcon(":/media/text.png"));
    mUi.twObjects->topLevelItem(2)->setIcon(0, QIcon(":/media/image.png"));
    mUi.twObjects->topLevelItem(3)->setIcon(0, QIcon(":/media/talk-baloon.png"));
    mUi.twObjects->topLevelItem(4)->setIcon(0, QIcon(":/media/button.png"));


    //scenes' widget
    //connect(mUi.scenesWidget, SIGNAL(itemDoubleClicked (QTreeWidgetItem *, int)), this, SLOT(onmUi.scenesWidgetDoubleClicked(QTreeWidgetItem *, int)));
    connect(mUi.scenesWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onScenesWidgetItemChanged(QTreeWidgetItem*,int)));
    connect(mUi.scenesWidget, SIGNAL(itemClicked (QTreeWidgetItem *, int)), this, SLOT(onSceneItemClicked(QTreeWidgetItem*, int)));
    connect(mUi.scenesWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onScenesWidgetCustomContextMenuRequested(const QPoint&)));
    //pause scenes widget
    connect(mUi.pauseScenesWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onScenesWidgetItemChanged(QTreeWidgetItem*,int)));
    connect(mUi.pauseScenesWidget, SIGNAL(itemClicked (QTreeWidgetItem *, int)), this, SLOT(onSceneItemClicked(QTreeWidgetItem*, int)));
    connect(mUi.pauseScenesWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onScenesWidgetCustomContextMenuRequested(const QPoint&)));

    connect(mUi.addSceneBtn, SIGNAL(clicked()), this, SLOT(addScene()));
    connect(mUi.delSceneBtn, SIGNAL(clicked()), this, SLOT(deleteScene()));
    connect(mUi.twObjects, SIGNAL(itemDoubleClicked (QTreeWidgetItem *, int)), this, SLOT(onTwObjectsDoubleClicked(QTreeWidgetItem *, int)));
    connect(mUi.runAction, SIGNAL(triggered()), this, SLOT(onRunTriggered()));
    connect(mUi.addPauseSceneBtn, SIGNAL(clicked()), this, SLOT(addScene()));
    connect(mUi.delPauseSceneBtn, SIGNAL(clicked()), this, SLOT(deleteScene()));


    //resources viewer
    mUi.resourcesTabWidget->setCurrentIndex(1);
    widget = mUi.resourcesTabWidget->currentWidget();
    mResourcesView = 0;
    if(widget) {
        layout = widget->layout() ? widget->layout() : new QVBoxLayout(widget);
        mResourcesView = new ResourcesView(widget);
        mResourcesView->setHeaderHidden(true);
        layout->addWidget(mResourcesView);

        connect(mResourcesView, SIGNAL(editResource(GameObject*)), this, SLOT(onEditResource(GameObject*)));
        connect(mResourcesView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onResourcesDoubleClicked(const QModelIndex&)));
    }

    mUi.resourcesTabWidget->setCurrentIndex(0);

    //objects viewer
    mObjectsView = new ObjectsView(this);
    widget = mUi.objectsViewWidget->widget();
    layout = widget->layout() ? widget->layout() : new QVBoxLayout(widget);
    layout->addWidget(mObjectsView);
    connect(mObjectsView, SIGNAL(objectSelected(Object*)), this, SLOT(onSelectedObjectChanged(Object*)));
    mUi.menuView->addAction(mUi.objectsViewWidget->toggleViewAction());
    mUi.objectsViewWidget->hide();

    addScene();

    int width = mUi.actionCatalogDockWidget->width()/2;
    width += mUi.resourcesDockWidget->width() / 2;
    mUi.propertiesDockWidget->setMinimumWidth(width);

    vLayout = mUi.actionCatalogDockWidget->layout();
    if (vLayout) {
        ActionCatalogWidget *actionCatalog = new ActionCatalogWidget(mUi.actionCatalogDockWidget);
        if (actionsModel) {
            //connect(this, SIGNAL(newAction(Action*)), actionsModel, SLOT(appendAction(Action*)));
            //connect(actionCatalog, SIGNAL(newAction(Action*)), actionsModel, SLOT(appendAction(Action*)));
            connect(actionCatalog, SIGNAL(newAction(Action*)), this, SLOT(onNewAction(Action*)));
        }

        connect(actionCatalog, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onActionCatalogClicked(const QModelIndex&) ));
        vLayout->addWidget(actionCatalog);
    }


    //connect actions' signals with the respective slots
    connect(mUi.newProjectAction, SIGNAL(triggered()), this, SLOT(newProject()));
    connect(mUi.openProjectAction, SIGNAL(triggered()), this, SLOT(openFileOrProject()));
    connect(mUi.aboutAction, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
    connect(mUi.saveProjectAction, SIGNAL(triggered()), this, SLOT(saveProject()));
    connect(mUi.exportProject, SIGNAL(triggered()), this, SLOT(exportProject()));

    //scene's buttons
    connect(mUi.upSceneBtn, SIGNAL(clicked()), this, SLOT(onSceneUpped()));
    connect(mUi.downSceneBtn, SIGNAL(clicked()), this, SLOT(onSceneDowned()));
    connect(mUi.upPauseSceneBtn, SIGNAL(clicked()), this, SLOT(onSceneUpped()));
    connect(mUi.downPauseSceneBtn, SIGNAL(clicked()), this, SLOT(onSceneDowned()));


    //scene's treewidget actions
    mCopyScene = new QAction(QIcon(":/media/editcopy.png"), tr("Copy"), mUi.scenesWidget);
    mCopyScene->setShortcut(QKeySequence::Copy);
    mCopyScene->setShortcutContext(Qt::WidgetShortcut);
    mUi.scenesWidget->addAction(mCopyScene);
    connect(mCopyScene, SIGNAL(triggered()), this, SLOT(copyScene()));

    mCutScene = new QAction(QIcon(":/media/editcut.png"), tr("Cut"), mUi.scenesWidget);
    mCutScene->setShortcut(QKeySequence::Cut);
    mCutScene->setShortcutContext(Qt::WidgetShortcut);
    mUi.scenesWidget->addAction(mCutScene);
    connect(mCutScene, SIGNAL(triggered()), this, SLOT(cutScene()));

    mPasteScene = new QAction(QIcon(":/media/editpaste.png"), tr("Paste"), mUi.scenesWidget);
    mPasteScene->setShortcut(QKeySequence::Paste);
    mPasteScene->setShortcutContext(Qt::WidgetShortcut);
    mUi.scenesWidget->addAction(mPasteScene);
    connect(mPasteScene, SIGNAL(triggered()), this, SLOT(pasteScene()));

    mDeleteScene = new QAction(QIcon(":/media/delete.png"), tr("Delete"), mUi.scenesWidget);
    mDeleteScene->setShortcut(QKeySequence::Delete);
    mDeleteScene->setShortcutContext(Qt::WidgetShortcut);
    mUi.scenesWidget->addAction(mDeleteScene);
    connect(mDeleteScene, SIGNAL(triggered()), this, SLOT(deleteScene()));

    restoreSettings();
    loadDefaultGame();
}

void Belle::saveSettings()
{
    //save settings
    mSettings->beginGroup("Window");
    mSettings->setValue("Geometry", this->saveGeometry());
    mSettings->setValue("State", this->saveState());
    mSettings->endGroup();
    mSettings->beginGroup("Project");
    if (Engine::isValid())
        mSettings->setValue("enginePath", Engine::path());
    if (! Engine::browserPath().isEmpty())
        mSettings->setValue("browser", Engine::browserPath());
    mSettings->setValue("useBuiltinBrowser", Engine::useBuiltinBrowser());
    mSettings->endGroup();
}

void Belle::restoreSettings()
{
    //load settings
    mSettings = new QSettings("Belle", "Belle", this);

    if (mSettings->contains("Window/Geometry"))
        this->restoreGeometry(mSettings->value("Window/Geometry").toByteArray());
    if (mSettings->contains("Window/State"))
        this->restoreState(mSettings->value("Window/State").toByteArray());
    if (mSettings->contains("Project/enginePath")) {
        if (Engine::isValidPath(mSettings->value("Project/enginePath").toString()))
            Engine::setPath(mSettings->value("Project/enginePath").toString());
    }
    if (mSettings->contains("Project/browser"))
        Engine::setBrowserPath(mSettings->value("Project/browser").toString());
    if (mSettings->contains("Project/useBuiltinBrowser"))
        Engine::setUseBuiltinBrowser(mSettings->value("Project/useBuiltinBrowser").toBool());
}

bool Belle::eventFilter(QObject *obj, QEvent *ev)
{
    return QMainWindow::eventFilter(obj, ev);
}

Belle::~Belle()
{
    ActionInfoManager::destroy();
    ResourceManager::destroy();
    EditorWidgetFactory::destroy();
    AssetManager::instance()->removeAssets();
    if (! mCurrentRunDirectory.isEmpty()) {
        QDir tmpdir(mCurrentRunDirectory);
        if (tmpdir.exists())
            tmpdir.removeRecursively();
    }

    saveSettings();
}

void Belle::onEditResource(GameObject* obj)
{
    if (! obj)
        return;

    Object* object = qobject_cast<Object*>(obj);
    if (object)
        mDrawingSurfaceWidget->setObject(object);

    removeWidgetsInPropertiesWidget();
    GameObjectEditorWidget* editor = EditorWidgetFactory::editorWidget(obj->type());
    if(editor) {
        editor->setGameObject(object);
        addWidgetToPropertiesWidget(editor);
    }
}

void Belle::onResourcesDoubleClicked(const QModelIndex& index)
{
    Scene* scene = mCurrentSceneManager->currentScene();

    if (! scene)
        return;

    const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(index.model());
    if (! model)
        return;

    GameObject* _resource = mResourcesView->object(index);
    if (! _resource)
        return;

    Object* resource = qobject_cast<Object*>(_resource);
    if (resource) {
        QVariantMap data(resource->toJsonObject(true));
        Object* object = ResourceManager::instance()->createObject(data, scene);
        if (object) {
            if (! object->resource())
                object->setResource(resource);
            scene->appendObject(object, true);
        }
    }
}

void Belle::onActionCatalogClicked(const QModelIndex& index)
{
   //mActionsView->appendAction();
}

void Belle::onScenesWidgetItemChanged(QTreeWidgetItem* item, int column)
{
    int index = item->treeWidget()->indexOfTopLevelItem(item);
    SceneManager* sceneManager = this->sceneManager(item->treeWidget()->objectName());
    Scene* currScene = sceneManager->sceneAt(index);

    if (! currScene || currScene->name() == item->text(0))
        return;

    bool ok = currScene->setName(item->text(0));
    if (!ok)
        item->setText(0, currScene->name());
}

void Belle::updateActions()
{
    if (currentScene()) {
        ActionsModel *model = qobject_cast<ActionsModel*> (mActionsView->model());
        if (model) {
            model->setActions(currentScene()->actions());
            connect(currentScene(), SIGNAL(actionAdded(Action*)), model, SLOT(appendAction(Action*)), Qt::UniqueConnection);
            connect(currentScene(), SIGNAL(actionRemoved(int)), model, SLOT(removeAction(int)), Qt::UniqueConnection);
        }
    }
}

void Belle::updateObjects()
{
    Scene * scene = currentScene();
    if (scene) {
        mObjectsView->clear();
        mObjectsView->addObjects(scene->objects());
        connect(scene, SIGNAL(objectAdded(Object*)), mObjectsView, SLOT(addObject(Object*)), Qt::UniqueConnection);
        connect(scene, SIGNAL(objectRemoved(Object*)), mObjectsView, SLOT(removeObject(Object*)), Qt::UniqueConnection);
    }
}

void Belle::addScene(Scene* scene, SceneManager* sceneManager)
{
    if (! sceneManager) {
        if (sender())
            sceneManager = this->sceneManager(sender()->objectName());
        else
            sceneManager = mCurrentSceneManager;
    }

    if (! sceneManager)
        return;

    //update previous scene icon
    if (sceneManager->currentScene())
        updateSceneIcon(sceneManager->currentScene());

    bool edit = false;
    if (! scene) {
        edit = true; //edit name after adding
        scene = sceneManager->addScene();
    }
    else
        sceneManager->addScene(scene);

    createSceneTreeItem(scenesWidget(sceneManager), scene, edit);
    updateSceneEditorWidget(scene);
    updateActions();
    updateObjects();
    mDrawingSurfaceWidget->update();
}

QTreeWidgetItem* Belle::createSceneTreeItem(QTreeWidget* widget, Scene* scene, bool edit)
{
    QTreeWidgetItem * item = new QTreeWidgetItem(widget, QStringList() << scene->objectName());
    widget->blockSignals(true);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setIcon(0, scene->icon());
    widget->blockSignals(false);
    widget->setCurrentItem(item);
    if (edit)
        widget->editItem(item);

    return item;
}

void Belle::onSceneRemoved(int index)
{
    QTreeWidget * scenesWidget = this->scenesWidget(sender()->objectName());
    if (index >= 0 && index < scenesWidget->topLevelItemCount())
        scenesWidget->takeTopLevelItem(index);
}

void Belle::onSceneNameChanged(int index, const QString& name)
{
    QTreeWidget * scenesWidget = this->scenesWidget(sender()->objectName());
    if (index >= 0 && index < scenesWidget->topLevelItemCount()) {
        QTreeWidgetItem* item = scenesWidget->topLevelItem(index);
        if (item)
            item->setText(0, name);
    }
}

void Belle::deleteScene()
{
    if (mCurrentSceneManager->count() == 1)
        return;

    QString senderName = sender()->objectName();
    if (senderName.isEmpty())
        senderName = mCurrentSceneManager->objectName();

    QTreeWidget * scenesWidget = this->scenesWidget(senderName);
    int index = scenesWidget->indexOfTopLevelItem(scenesWidget->currentItem());

    if (index != -1)
        mCurrentSceneManager->removeSceneAt(index, true);
}

void Belle::onSceneItemClicked(QTreeWidgetItem *item, int column)
{
    QTreeWidget *widget = item->treeWidget();
    SceneManager * currSceneManager = this->sceneManager(widget->objectName());
    int prevIndex = currSceneManager->currentSceneIndex();
    int currIndex = widget->indexOfTopLevelItem(item);

    mDrawingSurfaceWidget->setObject(0);

    if (currSceneManager != mCurrentSceneManager || prevIndex != currIndex) {
        updateSceneIcon(mCurrentSceneManager->currentScene()); //update current scene icon

        currSceneManager->setCurrentSceneIndex(currIndex);
        mCurrentSceneManager = currSceneManager;
        if (mCurrentSceneManager->currentScene())
            mCurrentSceneManager->currentScene()->show();

        updateActions();
        updateObjects();
        mDrawingSurfaceWidget->setSceneManager(currSceneManager);
        mDrawingSurfaceWidget->update();
    }

    updateSceneEditorWidget(mCurrentSceneManager->currentScene());
}

void Belle::updateSceneIcon(Scene* scene)
{
    //update previous icon
    if (scene) {
        scene->hide(); //focus out first to update pixmap
        QTreeWidgetItem *prevItem = scenesWidget(mCurrentSceneManager)->topLevelItem(mCurrentSceneManager->indexOf(scene));
        if (prevItem)
            prevItem->setIcon(0, scene->icon());
    }
}

void Belle::updateSceneEditorWidget(Scene* scene)
{
    if (scene) {
        GameObjectEditorWidget* editor = EditorWidgetFactory::editorWidget("scene");
        switchWidgetInPropertiesWidget(EditorWidgetFactory::editorWidget("scene"));
        if (editor) {
            editor->setGameObject(scene);
        }
    }
}

void Belle::onTwObjectsDoubleClicked(QTreeWidgetItem *item, int column)
{
    Scene *scene = currentScene();
    if (! scene)
        return;

    GameObject* resource = 0;
    int accepted = 0;
    QString startPath = QDir::currentPath();
    QString filter;
    QString path;
    AddCharacterDialog *dialog = 0;

    switch (item->treeWidget()->indexOfTopLevelItem(item))
    {
    //character
    case 0:
        dialog = new AddCharacterDialog();
        accepted = dialog->exec();
        if (accepted) {
            resource = new Character(dialog->name(), dialog->statesAndImagePaths(), ResourceManager::instance());
            ResourceManager::instance()->add(resource);
        }
        break;

        //TextBox
    case 1:
        resource = new TextBox(tr("Text goes here..."), ResourceManager::instance());
        ResourceManager::instance()->add(resource);
        break;

        //Image
    case 2:
        filter = tr("Images(*.png *.xpm *.jpg *.jpeg *.gif)");
        if (QDir(RESOURCES_DEFAULT_PATH).exists())
            startPath = RESOURCES_DEFAULT_PATH;

        path = QFileDialog::getOpenFileName(this, tr("Choose Image"), startPath, filter);
        if (path.isEmpty())
            break;
        resource = new Image(path, ResourceManager::instance());
        ResourceManager::instance()->add(resource);
        break;

       //Dialogue Box
    case 3:
        resource = new DialogueBox(ResourceManager::instance());
        ResourceManager::instance()->add(resource);
        break;

        //Button
    case 4:
        resource = new Button(ResourceManager::instance());
        ResourceManager::instance()->add(resource);
        break;
    }

    if (mResourcesView && resource) {
        mUi.resourcesTabWidget->setCurrentIndex(1);
        mResourcesView->select(resource->objectName());
    }

    if (dialog)
        dialog->deleteLater();
}

void Belle::onTwObjectsClicked(QTreeWidgetItem *, int)
{
    /*QLayout *layout = mUi.dockWidgetContents_5->layout();
    WidgetGroup * gwidget = new WidgetGroup(Qt::Vertical, mUi.dockWidgetContents_5);

    gwidget->beginLayout(Qt::Horizontal);
    gwidget->addWidget(new QLabel("Name: ", gwidget), true, QSizePolicy::Fixed);
    gwidget->addWidget(new QLineEdit(gwidget));
    gwidget->endLayout();
    gwidget->beginLayout(Qt::Horizontal);
    gwidget->addWidget(new QLabel("State: ", gwidget), true, QSizePolicy::Fixed);
    gwidget->addWidget(new QLineEdit(gwidget));
    gwidget->addWidget(new QPushButton(tr("Select Image"), gwidget), true, QSizePolicy::Fixed);
    gwidget->endLayout();
    layout->addWidget(gwidget);*/
}

void Belle::onSelectedObjectChanged(Object* obj)
{
    GameObjectEditorWidget* editor = 0;
    if (obj) {
        editor = EditorWidgetFactory::editorWidget(obj->type());
        switchWidgetInPropertiesWidget(editor);
        if (editor)
            editor->setGameObject(obj);
    }
    else if (mCurrentSceneManager && mCurrentSceneManager->currentScene()) {
        editor = EditorWidgetFactory::editorWidget("scene");
        switchWidgetInPropertiesWidget(editor);
    }
    else
        switchWidgetInPropertiesWidget(0);
}


void Belle::onActionsViewClicked(const QModelIndex& index)
{
    if (! index.isValid())
        return;

    const ActionsModel *model = qobject_cast<const ActionsModel*>(index.model());
    if (! model)
        return;

    Action* action = model->actionForIndex(index);
    if (! action)
        return;
    GameObjectEditorWidget* editor = 0;

    removeWidgetsInPropertiesWidget();
    editor = EditorWidgetFactory::editorWidget(action->type());
    if (editor) {
        editor->setGameObject(action);
        addWidgetToPropertiesWidget(editor);
    }

}


void Belle::onNewAction(Action * action)
{
    if (currentScene()) {
        QModelIndexList indexes = mActionsView->selectionModel()->selectedIndexes();
        int row = -1;
        if (indexes.size())
            row = indexes.last().row();
        if (row != -1)
            currentScene()->insertAction(row+1, action);
        else {
            currentScene()->appendAction(action);
            mActionsView->scrollToBottom();
        }
    }

}

void Belle::switchWidgetInPropertiesWidget(QWidget* widget)
{
    QWidget* currWidget = widgetInPropertiesWidget();
    if (widget == currWidget)
        return;

    removeWidgetsInPropertiesWidget();
    addWidgetToPropertiesWidget(widget);
}

void Belle::addWidgetToPropertiesWidget(QWidget* widget)
{
    QLayout *layout = mUi.dockWidgetContents_5->layout();
    if (layout && widget) {
        layout->addWidget(widget);
        if (widget->isHidden())
            widget->show();
        //widget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    }
}

QWidget* Belle::widgetInPropertiesWidget()
{
    QLayout *layout = mUi.dockWidgetContents_5->layout();
    if(layout) {
        QLayoutItem *item = layout->itemAt(0);
        if (item)
            return item->widget();
    }

    return 0;
}

void Belle::removeWidgetsInPropertiesWidget()
{
    QLayout *layout = mUi.dockWidgetContents_5->layout();
    if(layout) {
        QLayoutItem *item;
        while((item = layout->takeAt(0)) != 0) {
            if (item && item->widget()) {
                item->widget()->setParent(0);
                item->widget()->hide();
            }
        }
    }
}

void Belle::onRunTriggered()
{
    if (! checkEnginePath())
        return;

    QString exportedTo = exportProject(QDir::tempPath(), true);

    if (! exportedTo.isEmpty()) {

        mHttpServer.setServerDirectory(exportedTo);
        bool started = mHttpServer.start();
        if (! started) {
            QMessageBox::critical(this, tr("Couldn't start the server"), tr("The server couldn't find any free ports on your system, which is very odd."));
            return;
        }

        if (Engine::useBuiltinBrowser()) {
            if (mWebView->url() == mHttpServer.serverUrl())
                mWebView->reload();
            else
                mWebView->setUrl(mHttpServer.serverUrl());
            mWebView->show();
        }
        else {
            //Get browser specified by the user
            QString browserPath = Engine::browserPath();
            if (! browserPath.isEmpty())
                QProcess::startDetached(browserPath, QStringList() << mHttpServer.serverUrl());
            else //open file (html) with default application
                QDesktopServices::openUrl(QUrl(mHttpServer.serverUrl()));
        }
    }
}

QString Belle::exportProject(const QString& _path, bool toRun)
{
    if (! Engine::isValid()) {
        QMessageBox::critical(this, tr("Invalid engine directory"), tr("Please, first set a valid engine directory through the menu Novel>Properties"));
        return "";
    }

    QString path(_path);
    if (path.isEmpty())
         path = QFileDialog::getExistingDirectory(this, tr("Select Output Directory"));
    if (path.isEmpty())
        return "";

    QString fileName(GAME_FILENAME);
    QDir engineDir (Engine::path());
    QString title = mNovelData.value("title").toString();
    QDir projectDir(path);

    title = title.isEmpty() ? tr("Untitled") : title;

    if (! toRun || (toRun && mCurrentRunDirectory.isEmpty())) {
        title = Utils::newFileName(projectDir.absoluteFilePath(title));
        projectDir.mkdir(title);
        projectDir.cd(title);
    }

    if (toRun) {
        if (mCurrentRunDirectory.isEmpty())
            mCurrentRunDirectory = projectDir.absolutePath();
        projectDir = QDir(mCurrentRunDirectory);
    }

    //copy all engine files
    QStringList fileNames = engineDir.entryList(QStringList() << "*.js" << "*.html" << "*.css", QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    bool pathChanged = Engine::pathChanged();
    foreach(const QString&fileName, fileNames) {
        if (pathChanged && QFile::exists(projectDir.absoluteFilePath(fileName)))
            QFile::remove(projectDir.absoluteFilePath(fileName));
        QFile::copy(engineDir.absoluteFilePath(fileName), projectDir.absoluteFilePath(fileName));
    }

    //copy images, sounds and fonts in use
    AssetManager::instance()->save(projectDir);

    //export gameFile
    exportGameFile(projectDir.absoluteFilePath(fileName));

    return projectDir.absolutePath();
    //Utils::safeCopy(QDir::current().absoluteFilePath(fileName), projectDir.absoluteFilePath(fileName));
}

void Belle::saveProject()
{
    if (mSavePath.isEmpty() || ! QFile::exists(mSavePath)) {
        SaveProjectDialog saveProjectDialog(mNovelData.value("title").toString(), this);
        int res = saveProjectDialog.exec();
        if (res == QDialog::Accepted) {
            mSavePath = saveProjectDialog.projectPath();
            QDir parentDir = saveProjectDialog.projectParentDirectory();
            parentDir.mkdir(saveProjectDialog.projectName());
            changeProjectTitle(saveProjectDialog.projectName());
        }
        else
            return;
    }

    if (QFile::exists(mSavePath)) {
        QDir projectDir(mSavePath);
        //copy images and fonts in use
        AssetManager::instance()->save(projectDir);
        //export gameFile
        exportGameFile(projectDir.absoluteFilePath(GAME_FILENAME));

        if(statusBar())
            statusBar()->showMessage(tr("Project saved..."), 3000);
    }
}

QVariantMap Belle::readGameFile(const QString& filepath) const
{
    QVariantMap dataMap;

    QFile file(filepath);
    if (! file.open(QFile::ReadOnly))
        return dataMap;

    QByteArray contents = file.readAll();
    file.close();

    //if new game file format, just remove start ("game.data =")
    int i = 0;
    for(i=0; i < contents.size() && contents[i] != '{'; i++);
    contents = contents.mid(i);

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(contents, &error);
    if (error.error != QJsonParseError::NoError)
        return dataMap;

    QVariant data = doc.toVariant();
    if (data.type() != QVariant::Map)
        return dataMap;

    return data.toMap();
}

void Belle::clearProject()
{
    mDefaultSceneManager->removeScenes(true);
    mPauseSceneManager->removeScenes(true);
    ResourceManager::instance()->clear(true);
    AssetManager::instance()->setLoadPath("");
    AssetManager::instance()->removeAssets();
    mSavePath = "";
    mCurrentRunDirectory = "";
}

void Belle::openFileOrProject(QString filepath)
{
    QStringList filters;
    QDir saveDir;

    filters << tr("Game File") + "(game_data.js)"
            << "Javascript (*.js)";

    if (filepath.isEmpty())
        filepath = QFileDialog::getOpenFileName(this, tr("Select project file"), QDir::currentPath(), filters.join(";;"));

    if (filepath.isEmpty())
        return;

    QVariantMap object = readGameFile(filepath);

    if (object.isEmpty()) {
        QMessageBox::warning(this, tr("ERROR"),
                            tr("The game file could not be read correctly."));
        return;
    }

    clearProject();
    mSavePath = QFileInfo(filepath).absolutePath();
    saveDir = QFileInfo(filepath).absoluteDir();
    AssetManager::instance()->setLoadPath(mSavePath);
    setNovelProperties(object);
    AssetManager::instance()->load(QDir(mSavePath));
    ResourceManager::instance()->load(object);
    //FIXME: For backwards compatibility, remove at some point
    if (object.contains("customFonts") && object.value("customFonts").type() == QVariant::List) {
        QVariantList customFonts = object["customFonts"].toList();
        if (! customFonts.isEmpty()) {
            for(int i=0; i < customFonts.size(); i++)
                AssetManager::instance()->loadAsset(saveDir.absoluteFilePath(customFonts[i].toString()), Asset::Font);
        }
    }

    //pause screen import
    if (object.contains("pauseScreen") && object.value("pauseScreen").type() == QVariant::Map) {
        QVariantMap pauseScreen = object.value("pauseScreen").toMap();
        if (pauseScreen.contains("scenes") && pauseScreen.value("scenes").type() == QVariant::List) {
            QVariantList scenes = pauseScreen.value("scenes").toList();
            mCurrentSceneManager = mPauseSceneManager;
            importScenes(scenes, mPauseSceneManager);
            mPauseSceneManager->setCurrentSceneIndex(0);
        }
    }

    if (object.contains("scenes") && object.value("scenes").type() == QVariant::List) {
        QVariantList scenes = object.value("scenes").toList();
        mCurrentSceneManager = mDefaultSceneManager;
        importScenes(scenes, mDefaultSceneManager);
        mDefaultSceneManager->setCurrentSceneIndex(0);
        updateActions();
    }

    AssetManager::instance()->setLoadPath("");

    //Fix new object sync property
    if (! object.contains("version") || object.value("version").toInt() < (int) VERSION) {
        QMessageBox::warning(this,
                             tr("Project warning"),
                             tr("The project you are opening was created with an earlier version of Belle "
                                "and it may not load correctly."));

        int version = object.value("version", 0).toInt();
        if (version < (int) 0x000500) {
            _fixSync(mDefaultSceneManager->scenes());
            _fixSync(mPauseSceneManager->scenes());
        }
    }

}

//Temporary function that will fix new sync property in old projects
void Belle::_fixSync(const QList<Scene*>& scenes)
{
    QList<Object*> objects;
    for(int i=0; i < scenes.size(); i++) {
        objects = scenes[i]->objects();
        for(int j=0; j < objects.size(); j++) {
            objects[j]->setSync(false);
        }
    }
}

void Belle::importScenes(const QVariantList& scenes, SceneManager* sceneManager)
{
    Scene *scene = 0;

    for(int i=0; i < scenes.size(); i++) {
        if (scenes[i].type() == QVariant::Map) {
            scene = new Scene(scenes[i].toMap(), sceneManager);
            addScene(scene, sceneManager);
        }
    }
}

QVariantMap Belle::createGameFile() const
{
    QVariantMap jsonFile;
    QString font = Utils::font(mNovelData.value("fontSize").toInt(), mNovelData.value("fontFamily").toString());
    QMapIterator<QString, QVariant> it(mNovelData);

    while(it.hasNext()){
        it.next();
        jsonFile.insert(it.key(), it.value());
    }

    //remove "fontSize" and "fontFamily" attributes, they are going to be replaced by the "font" attribute
    jsonFile.insert("font", font);
    jsonFile.remove("fontSize");
    jsonFile.remove("fontFamily");
    jsonFile.insert("version", VERSION);

    QVariantMap resourcesData = ResourceManager::instance()->toMap();
    jsonFile.insert("resources", resourcesData);

    QVariantList scenes;
    for (int i=0; i < mDefaultSceneManager->size(); i++) {
        scenes.append(mDefaultSceneManager->sceneAt(i)->toJsonObject(false));
    }

    jsonFile.insert("scenes", scenes);

    //export pause screen scenes
    scenes.clear();
    for (int i=0; i < mPauseSceneManager->size(); i++) {
        scenes.append(mPauseSceneManager->sceneAt(i)->toJsonObject(false));
    }

    QVariantMap pauseScreen;
    pauseScreen.insert("scenes", scenes);
    jsonFile.insert("pauseScreen", pauseScreen);

    return jsonFile;
}

void Belle::exportGameFile(const QString& path)
{
    QString filepath(path);

    if (filepath.isNull() || filepath.isEmpty()) {
        QString dirpath = QFileDialog::getExistingDirectory(this, tr("Choose the directory to export the game file to"),
                                                        "",
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);

        if (dirpath.isEmpty())
            return;

        filepath = QDir(dirpath).absoluteFilePath(GAME_FILENAME);
    }

    QFile file(filepath);

    if (! file.open(QFile::WriteOnly))
        return;

    QVariantMap jsonFile = createGameFile();

    file.write("game.data = ");
    file.write(QJsonDocument::fromVariant(jsonFile).toJson(QJsonDocument::Compact));
    file.close();
}

void Belle::showAboutDialog()
{
    QDialog *dialog = new AboutDialog();
    dialog->exec();
    dialog->deleteLater();
}

void Belle::onScenesWidgetCustomContextMenuRequested(const QPoint& point)
{
    QTreeWidget *treeWidget = qobject_cast<QTreeWidget*>(sender());
    if (! treeWidget)
        return;

    QMenu menu;
    SceneManager * sceneManager = this->sceneManager(treeWidget->objectName());
    mCurrentSceneManager = sceneManager;

    if (treeWidget->itemAt(point)) {
        menu.addAction(mCopyScene);
        menu.addAction(mCutScene);
    }

    if (sceneManager->clipboard()->hasScenes())
        menu.addAction(mPasteScene);

    if (sceneManager->scenes().count() > 1)
        menu.addAction(mDeleteScene);

    if (menu.actions().size())
        menu.exec(treeWidget->mapToGlobal(point));
}

void Belle::copyScene()
{
    QList<QObject*> scenes;
    int index = 0;
    Scene* scene = 0;

    QTreeWidget * scenesWidget = this->scenesWidget(mCurrentSceneManager->objectName());

    foreach(QTreeWidgetItem* item , scenesWidget->selectedItems()) {
        index = scenesWidget->indexOfTopLevelItem(item);
        scene = mCurrentSceneManager->sceneAt(index);
        if (scene)
            scenes.append(scene);
    }

    mCurrentSceneManager->clipboard()->add(scenes, Clipboard::Copy);
}

void Belle::cutScene()
{
    mCurrentSceneManager->clipboard()->add(mCurrentSceneManager->currentScene(), Clipboard::Cut);
}

void Belle::pasteScene()
{
    Clipboard::Operation op = mCurrentSceneManager->clipboard()->operation();
    QList<Scene*> scenes = mCurrentSceneManager->clipboard()->scenes();

    if (scenes.isEmpty())
        return;

    QTreeWidget* widget = this->scenesWidget(mCurrentSceneManager->objectName());
    int index = widget->indexOfTopLevelItem(widget->currentItem());

    foreach(Scene* scene, scenes) {

        if (op == Clipboard::Copy) {
            mCurrentSceneManager->insertScene(index, scene->copy());
        }
        else if (op == Clipboard::Cut) {
            mCurrentSceneManager->insertScene(index, scene);
        }
        else
            continue;

        index += 1;
    }

    updateScenesWidget(widget, index-1, true, true);
    if (op == Clipboard::Cut)
        SceneManager::clipboard()->clear();
}


void Belle::onSceneUpped()
{
    SceneManager* sceneManager = this->sceneManager(sender()->objectName());
    Scene * scene = sceneManager->currentScene();
    int currIndex = sceneManager->currentSceneIndex();

    if (! scene || currIndex == 0)
        return;

    sceneManager->insertScene(currIndex-1, scene);

    updateScenesWidget(this->scenesWidget(sceneManager->objectName()), currIndex-1, true);
}

void Belle::onSceneDowned()
{
    SceneManager* sceneManager = this->sceneManager(sender()->objectName());
    Scene * scene = sceneManager->currentScene();
    int currIndex = sceneManager->currentSceneIndex();
    if (! scene || currIndex == sceneManager->size()-1)
        return;

    sceneManager->insertScene(currIndex+1, scene);

    updateScenesWidget(this->scenesWidget(sceneManager->objectName()), currIndex+1, true);
}

void Belle::updateScenesWidget(QTreeWidget* widget, int currIndex, bool select, bool edit)
{
    widget->clear();
    QList<Scene*> scenes = this->sceneManager(widget->objectName())->scenes();

    for(int i=0; i < scenes.size(); i++) {
       createSceneTreeItem(widget, scenes[i]);
    }

    if (currIndex >= 0 && currIndex < scenes.size()) {
        QTreeWidgetItem* item = widget->topLevelItem(currIndex);
        if (select)
            widget->setCurrentItem(item);
        if (edit)
            widget->editItem(item);
    }
}

void Belle::onPropertiesTriggered()
{
    NovelPropertiesDialog dialog(mNovelData);

    dialog.exec();

    if (dialog.result() == QDialog::Accepted) {
        QVariantMap data = dialog.novelData();
        checkGameSize(data);
        setNovelProperties(data);

        Engine::setPath(dialog.enginePath());
        Engine::setBrowserPath(dialog.browserPath());
        Engine::setUseBuiltinBrowser(dialog.useBuiltinBrowser());
    }
}

void Belle::changeProjectTitle(const QString & name)
{
    if (name == mNovelData.value("title").toString())
        return;
    mNovelData.insert("title", name);
    setWindowTitle("Belle - " + name);
}


bool Belle::checkEnginePath()
{
    QString path("");

    if (! Engine::isValid())
        path = QFileDialog::getExistingDirectory(this, tr("Choose engine directory"), Engine::defaultPath());
    else
        path = Engine::path();

    if (! Engine::isValidPath(path)) {
        QMessageBox::critical(this, tr("Invalid engine directory"), tr("The directory you choose doesn't seem to be a valid engine directory."));
        return false;
    }

    Engine::setPath(path);
    return true;
}

void Belle::checkGameSize(const QVariantMap& data)
{
    int width = Scene::width(), height = Scene::height();
    if (data.contains("width") && data.value("width").canConvert(QVariant::Int)) {
        width = data.value("width").toInt();
    }

    if (data.contains("height") && data.value("height").canConvert(QVariant::Int)) {
        height = data.value("height").toInt();
    }

    if (width != Scene::width() || height != Scene::height())
        updateGameElements(width, height);
}

void Belle::updateGameElements(int w, int h)
{
    UpdateElementsDialog dialog(this);
    dialog.exec();
    if (dialog.result() == QDialog::Accepted) {
        bool pos = dialog.ui.updatePositionsCheckBox->isChecked();
        bool size = dialog.ui.updateSizesCheckBox->isChecked();
        mDefaultSceneManager->resizeScenes(w, h, pos, size);
        mPauseSceneManager->resizeScenes(w, h, pos, size);
    }
}

void Belle::setNovelProperties(const QVariantMap& _data)
{
    QVariantMap data;
    QMapIterator<QString, QVariant> it(_data);
    while(it.hasNext()) {
        it.next();
        if (it.value() != mNovelData.value(it.key()))
            data.insert(it.key(), it.value());
    }

    if (data.contains("title") && data.value("title").type() == QVariant::String) {
        changeProjectTitle(data.value("title").toString());
    }

    if (data.contains("width") && data.value("width").canConvert(QVariant::Int)) {
        int width = data.value("width").toInt();
        mNovelData.insert("width", width);
        mWebView->setFixedWidth(width);
        Scene::setWidth(width);
        //SceneManager::setSceneWidth(width);
        mDrawingSurfaceWidget->setFixedWidth(width);
    }

    if (data.contains("height") && data.value("height").canConvert(QVariant::Int)) {
        int height = data.value("height").toInt();
        mNovelData.insert("height", height);
        mWebView->setFixedHeight(height);
        Scene::setHeight(height);
        //SceneManager::setSceneHeight(height);
        mDrawingSurfaceWidget->setFixedHeight(height);
    }

    if (data.contains("textSpeed") && data.value("textSpeed").canConvert(QVariant::Int)) {
        mNovelData.insert("textSpeed", data.value("textSpeed").toInt());
    }

    if (data.contains("font") && data.value("font").type() == QVariant::String) {
        int fontSize = Utils::fontSize(data.value("font").toString());
        QString fontFamily = Utils::fontFamily(data.value("font").toString());
        Object::setDefaultFontSize(fontSize);
        Object::setDefaultFontFamily(fontFamily);
        mNovelData.insert("fontSize", fontSize);
        mNovelData.insert("fontFamily", fontFamily);
    }

    if (data.contains("fontSize") && data.value("fontSize").canConvert(QVariant::Int)) {
        Object::setDefaultFontSize(data.value("fontSize").toInt());
        mNovelData.insert("fontSize", data.value("fontSize").toInt());
    }

    if (data.contains("fontFamily") && data.value("fontFamily").type() == QVariant::String) {
        Object::setDefaultFontFamily(data.value("fontFamily").toString());
        mNovelData.insert("fontFamily", data.value("fontFamily").toString());
    }
}

Scene* Belle::currentScene()
{
    if (mCurrentSceneManager)
        return mCurrentSceneManager->currentScene();
    return 0;
}

SceneManager* Belle::sceneManager(const QString& name)
{
    if (name.contains("pause", Qt::CaseInsensitive))
        return mPauseSceneManager;
    return mDefaultSceneManager;
}

QTreeWidget* Belle::scenesWidget(const QString& name)
{
    if (name.contains("pause", Qt::CaseInsensitive))
        return mUi.pauseScenesWidget;
    return mUi.scenesWidget;
}

QTreeWidget* Belle::scenesWidget(SceneManager* sceneManager)
{
    return this->scenesWidget(sceneManager->objectName());
}

Belle* Belle::instance()
{
    return mInstance;
}

bool Belle::hasChanges() const
{
    if (mSavePath.isEmpty())
        return true;

    QDir projectDir(mSavePath);
    if (! projectDir.exists(GAME_FILENAME))
        return true;

    QVariantMap savedGame = readGameFile(projectDir.absoluteFilePath(GAME_FILENAME));
    QVariantMap currGame = createGameFile();
    if (savedGame != currGame)
        return true;
    return false;
}

bool Belle::confirmQuit(const QString& title, const QString& text)
{
    if (hasChanges()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, title, text,
                                      QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Cancel);

        if (reply == QMessageBox::Yes)
            saveProject();

        if (reply == QMessageBox::Yes || reply == QMessageBox::No)
            return true;

        return false;
    }

    return true;
}

void Belle::closeEvent(QCloseEvent *event)
{
    bool confirmed = confirmQuit(tr("Quit?"), tr("You have unsaved changes.\nDo you want to save changes before closing?"));

    if(! confirmed)
        event->ignore();
}

bool Belle::loadDefaultGame()
{
    QDir def(Engine::defaultPath());
    if (def.exists(GAME_FILENAME)) {
        openFileOrProject(def.absoluteFilePath(GAME_FILENAME));
        mSavePath = "";
        return true;
    }

    return false;
}

void Belle::newProject()
{
    bool confirmed = confirmQuit(tr("New Project"), tr("You have unsaved changes.\nDo you want to save changes before creating a new project?"));

    if (confirmed) {
        clearProject();
        bool loaded = loadDefaultGame();
        if (! loaded)
            addScene();
    }
}

Clipboard* Belle::clipboard() const
{
    return mClipboard;
}
