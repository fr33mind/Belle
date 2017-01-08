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

#include "actions_view.h"

#include <QStandardItemModel>
#include <QDebug>
#include <QBrush>
#include <QPointF>
#include <QRect>
#include <QFontMetrics>
#include <QApplication>
#include <QPalette>
#include <QVariant>
#include <QMenu>
#include <QTextEdit>

#include "scene_manager.h"
#include "actions_model.h"
#include "action.h"
#include "belle.h"
#include "gameobjectfactory.h"
#include "mimedata.h"

ActionsViewDelegate::ActionsViewDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{
}

void ActionsViewDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QStyledItemDelegate::paint(painter, option, index);

    const ActionsModel *model = qobject_cast<const ActionsModel*> (index.model());
    if (! model)
        return;

    const Action* action = model->actionForIndex(index);
    if (! action)
        return;

    QListView *view = qobject_cast<QListView*>(parent());
    if (! view)
        return;

    QRect rect = option.rect;
    QRect textRect = option.rect;
    rect.setWidth(rect.width()-1);
    textRect.setX(BORDER);
    QSize size(option.rect.width(), option.rect.height());

    QPointF startPoint (size.width()/2, 0);
    QPointF endPoint (size.width()/2, rect.y() + size.height());
    QLinearGradient linearGradient(startPoint, endPoint);
    linearGradient.setColorAt(0, QColor(255, 0, 0, 100));
    linearGradient.setColorAt(1, Qt::white);

    painter->save();
    painter->setBrush(QBrush(linearGradient));
    painter->drawRoundedRect(rect, 4, 4);
    painter->restore();

    if (option.state.testFlag(QStyle::State_Selected)) {
        painter->save();
        painter->setBrush(option.palette.brush(QPalette::Highlight));
        painter->drawRoundedRect(rect, 4, 4);
        painter->restore();
    }

    if (option.state.testFlag(QStyle::State_MouseOver)) {
        painter->save();
        //QPen pen(option.palette.brush(QPalette::Highlight), ITEM_SELECTED_LINE_WIDTH);
        //painter->setPen(pen);
        //painter->setBrush(QBrush());
        QBrush brush(option.palette.brush(QPalette::Highlight));
        QColor color = brush.color();
        int inc = 40;
        if (color.red()+inc < 255)
            color.setRed(color.red()+inc);
        else
            color.setRed(255);

        if (color.green()+inc < 255)
            color.setGreen(color.green()+inc);
        else
            color.setGreen(255);

        if (color.blue()+inc < 255)
            color.setBlue(color.blue()+inc);
        else
            color.setBlue(255);

        color.setAlpha(100);

        brush.setColor(color);
        painter->setBrush(brush);
        painter->drawRoundedRect(rect, 4, 4);
        painter->restore();
    }

    const GameObjectMetaType* metatype = GameObjectMetaType::metaType(action->type());
    const QIcon typeIcon = metatype ? metatype->icon() : QIcon();
    int textHeight = option.fontMetrics.size(0, action->title()).height();
    typeIcon.paint(painter, textRect.x(), textRect.y(), textHeight, textHeight);

    textRect.setX(textHeight+BORDER*2);
    painter->drawText(textRect, action->title());
    QString displayText = action->displayText();

    if (! displayText.isEmpty()) {
        textRect.setY(textRect.y() + textHeight);
        QStringList lines = displayText.split("\n");
        QString line;
        int lineWidth = 0;
        int marginWidth = option.fontMetrics.width("...") + 2;

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

QSize ActionsViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = option.rect.size();

    const ActionsModel *model = qobject_cast<const ActionsModel*> (index.model());
    if (! model)
        return size;

    const Action* action = model->actionForIndex(index);
    if (! action)
        return size;

    int height = 0;

    height += option.fontMetrics.size(0, action->name()).height() + BORDER;
    QString displayText = action->displayText();
    if (! displayText.isEmpty())
        height += option.fontMetrics.size(0, displayText).height() + BORDER;

    size.setHeight(height);

    return size;
}

QWidget* ActionsViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QTextEdit* widget = new QTextEdit(parent);
    QRect rect = option.rect;
    widget->move(rect.x(), rect.y());
    widget->resize(rect.size());
    return widget;
}

void ActionsViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QTextEdit* lineEdit = qobject_cast<QTextEdit*>(editor);
    const ActionsModel* actionsModel = qobject_cast<const ActionsModel*>(index.model());
    if (lineEdit && actionsModel) {
        Action* action = actionsModel->actionForIndex(index);
        if (action)
            lineEdit->setText(action->editText());
    }
}

void ActionsViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QTextEdit* lineEdit = qobject_cast<QTextEdit*>(editor);
    const ActionsModel* actionsModel = qobject_cast<const ActionsModel*>(index.model());
    if (lineEdit && actionsModel) {
        Action* action = actionsModel->actionForIndex(index);
        if (action)
            action->setEditText(lineEdit->toPlainText());
    }
}

bool ActionsViewDelegate::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return  && keyEvent->modifiers() == Qt::ControlModifier) {
            QTextEdit* textEdit = qobject_cast<QTextEdit*>(watched);
            if (textEdit) {
                emit commitData(textEdit);
                emit closeEditor(textEdit);
                return true;
            }
        }
    }

    return QStyledItemDelegate::eventFilter(watched, event);
}

ActionsView::ActionsView(QWidget *parent) :
    QListView(parent)
{
    ActionsModel* model = new ActionsModel(this);
    mActionsModel = model;
    this->setModel(model);

    setDragEnabled(true);
    setEditTriggers(QAbstractItemView::DoubleClicked);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDragDropOverwriteMode(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    QStyledItemDelegate* delegate = new ActionsViewDelegate(this);
    setItemDelegate(delegate);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    //actions
    mCopyAction = new QAction(QIcon(":/media/editcopy.png"), tr("Copy"), this);
    mCopyAction->setShortcut(QKeySequence("Ctrl+C"));
    mCopyAction->setShortcutContext(Qt::WidgetShortcut);
    addAction(mCopyAction);

    mCutAction = new QAction(QIcon(":/media/editcut.png"), tr("Cut"), this);
    mCutAction->setShortcut(QKeySequence("Ctrl+X"));
    mCutAction->setShortcutContext(Qt::WidgetShortcut);
    addAction(mCutAction);

    mPasteAction = new QAction(QIcon(":/media/editpaste.png"), tr("Paste"), this);
    mPasteAction->setShortcut(QKeySequence("Ctrl+V"));
    mPasteAction->setShortcutContext(Qt::WidgetShortcut);
    addAction(mPasteAction);

    mDeleteAction = new QAction(QIcon(":/media/delete.png"), tr("Delete"), this);
    mDeleteAction->setShortcut(QKeySequence::Delete);
    mDeleteAction->setShortcutContext(Qt::WidgetShortcut);
    addAction(mDeleteAction);

    connect(mCopyAction, SIGNAL(triggered()), this, SLOT(onCopyAction()));
    connect(mCutAction, SIGNAL(triggered()), this,  SLOT(onCutAction()));
    connect(mPasteAction, SIGNAL(triggered()), this, SLOT(onPasteAction()));
    connect(mDeleteAction, SIGNAL(triggered()), this, SLOT(onDeleteAction()));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onContextMenuRequested(const QPoint&)));

    connect(this, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onItemClicked(const QModelIndex&)));
    connect(delegate, SIGNAL(closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)), this, SLOT(onEditorClosed(QWidget*, QAbstractItemDelegate::EndEditHint)));
}

void ActionsView::onContextMenuRequested(const QPoint & point)
{
    QMenu menu;

    if (! selectedIndexes().isEmpty()) {
        menu.addAction(mCopyAction);
        menu.addAction(mCutAction);
        menu.addSeparator();
        menu.addAction(mDeleteAction);
    }
    else {
         menu.addAction(mPasteAction);
    }
    menu.exec(mapToGlobal(point));
}

void ActionsView::onDeleteAction()
{
    Scene* scene = Belle::instance()->currentScene();
    if (scene) {
        setAutoScroll(false);
        QModelIndexList indexes = selectedIndexes();
        qSort(indexes);
        for(int i=indexes.size()-1; i >= 0; --i) {
            scene->removeActionAt(indexes[i].row(), true);
        }
        setAutoScroll(true);
    }
}

void ActionsView::onCopyAction()
{
    QList<QObject*> objects;
    foreach(Action* action, selectedActions())
        objects.append(action);
    if (! objects.isEmpty())
        Belle::instance()->clipboard()->add(objects, Clipboard::Copy);
}

void ActionsView::onCutAction()
{
    QList<QObject*> objects;
    foreach(Action* action, selectedActions())
        objects.append(action);
    if (! objects.isEmpty())
        Belle::instance()->clipboard()->add(objects, Clipboard::Cut);
}

void ActionsView::onPasteAction()
{
    Clipboard* clipboard = Belle::instance()->clipboard();
    QList<Action*> actions = clipboard->actions();
    Scene* currScene = Belle::instance()->currentScene();
    QModelIndexList indexes = selectedIndexes();
    QModelIndex lastIndex = indexes.isEmpty() ? QModelIndex() : indexes.last();
    int insertRow = lastIndex.row();
    if (! currScene || actions.isEmpty())
        return;

    Action* firstAction = actions.first();
    Scene* scene = firstAction ? firstAction->scene() : 0;
    int actionsSize = actions.size();
    int actionIndex = insertRow;
    if (scene) {
        actionIndex = scene->indexOf(firstAction);
        if (insertRow > actionIndex) {
            insertRow++;
        }
    }

    if (clipboard->operation() == Clipboard::Copy) {
        pasteActionsAt(insertRow, actions, true);
    }
    else if (clipboard->operation() == Clipboard::Cut) {
        if (actionIndex < insertRow) {
            int dif = insertRow - actionIndex;
            if (dif > actionsSize)
                dif = actionsSize;
            insertRow -= dif;
        }

        foreach(Action* action, actions)
            if(action->scene())
                action->scene()->removeAction(action);

        pasteActionsAt(insertRow, actions, false);
    }

    //To mimic the usual cut and paste behaviour
    clipboard->setOperation(Clipboard::Copy);
}

void ActionsView::appendAction(Action* action)
{
    QStandardItemModel* model = qobject_cast<QStandardItemModel*> (this->model());
    if (! model)
        return;

    if (action) {
        const GameObjectMetaType* metatype = GameObjectMetaType::metaType(action->type());
        const QIcon typeIcon = metatype ? metatype->icon() : QIcon();
        model->appendRow(new QStandardItem(typeIcon, action->name()));
    }
}

void ActionsView::dropEvent(QDropEvent *event)
{
    QListView::dropEvent(event);
    const MimeData *mimeData = dynamic_cast<const MimeData*> (event->mimeData());
    if (! mimeData)
        return;

    QList<QObject*> objects = mimeData->objects();
    QList<Action*> actions;
    foreach(QObject* obj, objects) {
        Action* action = qobject_cast<Action*>(obj);
        if (action)
            actions.append(action);
    }

    selectActions(actions);
}

void ActionsView::onItemClicked(const QModelIndex & index)
{
    if (parent() && qobject_cast<QWidget*>(parent())) {
        this->setFocus();
    }

    if (mActionsModel) {
        Action* action = mActionsModel->actionForIndex(index);
        //Forces selected action's editor widget to appear
        if (action == mActionsModel->currentAction())
            emit currentActionClicked(action);
    }
}

QList<Action*> ActionsView::selectedActions() const
{
    QList<Action*> actions;
    Scene* scene = Belle::instance()->currentScene();
    if (scene) {
        QModelIndexList indexes = selectedIndexes();
        qSort(indexes);
        for(int i=0; i < indexes.size(); i++)
            actions.append(scene->actionAt(indexes[i].row()));
    }

    return actions;
}

void ActionsView::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected )
{
    QListView::selectionChanged(selected, deselected);

    if (!mActionsModel)
        return;

    QModelIndexList indexes;
    Action* action = mActionsModel->currentAction();

    if (!deselected.isEmpty()) {
        indexes = deselected.indexes();
        if (mActionsModel->currentAction() == mActionsModel->actionForIndex(indexes.first()))
            action = 0;
    }

    if (!selected.isEmpty()) {
        indexes = selected.indexes();
        action = mActionsModel->actionForIndex(indexes.first());
    }

    if (!action) {
        indexes = selectedIndexes();
        if (!indexes.isEmpty())
            action = mActionsModel->actionForIndex(indexes.first());
    }

    mActionsModel->setCurrentAction(action);
    emit currentActionChanged(mActionsModel->currentAction());
}

void ActionsView::selectAction(Action * action)
{
    if (!mActionsModel)
        return;

    QModelIndex index = mActionsModel->indexForAction(action);
    selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    scrollTo(index);
}

void ActionsView::selectActions(const QList<Action*>& actions)
{
    if (actions.isEmpty())
        return;

    Action* firstAction = actions.first();
    Action* lastAction = actions.last();
    QModelIndex topLeft = mActionsModel->indexForAction(firstAction);
    QModelIndex bottomRight = mActionsModel->indexForAction(lastAction);
    QItemSelection selection(topLeft, bottomRight);
    selectionModel()->select(selection, QItemSelectionModel::ClearAndSelect);
    selectionModel()->setCurrentIndex(topLeft, QItemSelectionModel::NoUpdate);
}

void ActionsView::pasteActionsAt(int index, const QList<Action *> & actions, bool copy, bool select)
{
    Scene* currScene = Belle::instance()->currentScene();
    if (!currScene)
        return;

    Action* firstAction = 0;
    int i = 0;
    foreach(Action* action, actions) {
        if (copy)
            action = GameObjectFactory::createAction(action->toJsonObject(), this);

        if (!firstAction)
            firstAction = action;

        if (index >= 0)
            currScene->insertAction(index+i, action);
        else
            currScene->appendAction(action);
        i++;
    }

    if (select)
        selectAction(firstAction);
}

void ActionsView::onEditorClosed(QWidget * editor, QAbstractItemDelegate::EndEditHint hint)
{
    selectionModel()->clearSelection();
    selectionModel()->select(currentIndex(), QItemSelectionModel::ClearAndSelect);
}
