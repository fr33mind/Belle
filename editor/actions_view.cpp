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
#include <QTextBlock>

#include "scene_manager.h"
#include "actions_model.h"
#include "action.h"
#include "belle.h"
#include "gameobjectfactory.h"
#include "mimedata.h"
#include "actionpainterfactory.h"

ActionsViewDelegate::ActionsViewDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{
    mTextEditCursorAtEnd = false;
    mTextEditCursorBlockPos = -1;
}

void ActionsViewDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QStyledItemDelegate::paint(painter, option, index);

    const ActionsModel *model = qobject_cast<const ActionsModel*> (index.model());
    if (! model)
        return;

    Action* action = model->actionForIndex(index);
    if (! action)
        return;

    QListView *view = qobject_cast<QListView*>(parent());
    if (! view)
        return;

    ActionPainter* actionPainter = ActionPainterFactory::actionPainter(action->type());
    if (!actionPainter)
        return;

    actionPainter->setAction(action);
    actionPainter->paint(painter, option, index);
}

QSize ActionsViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = option.rect.size();

    const ActionsModel *model = qobject_cast<const ActionsModel*> (index.model());
    if (! model)
        return size;

    Action* action = model->actionForIndex(index);
    if (! action)
        return size;

    ActionPainter* actionPainter = ActionPainterFactory::actionPainter(action->type());
    if (!actionPainter)
        return size;

    actionPainter->setAction(action);
    return actionPainter->sizeHint(option, index);
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
        if (action) {
            lineEdit->setText(action->editText());
            QTextCursor cursor = lineEdit->textCursor();
            if (mTextEditCursorAtEnd)
                cursor.movePosition(QTextCursor::End);
            if (mTextEditCursorBlockPos >= 0) {
                int maxBlockPos = cursor.block().length() - 1;
                int cursorBlockPos = mTextEditCursorBlockPos;
                if (cursorBlockPos > maxBlockPos)
                    cursorBlockPos = maxBlockPos;
                cursor.setPosition(cursor.block().position()+cursorBlockPos);
            }

            lineEdit->setTextCursor(cursor);

            bool editNextItem = true,
                 editPrevItem = true;

            if (index.row() == 0) {
                editPrevItem = false;
            }
            else if (index.row() == actionsModel->rowCount()-1) {
                editNextItem = false;
            }

            lineEdit->setProperty("editNextItem", editNextItem);
            lineEdit->setProperty("editPrevItem", editPrevItem);
        }
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
        QTextEdit* textEdit = qobject_cast<QTextEdit*>(watched);

        if (textEdit) {
            mTextEditCursorAtEnd = false;
            mTextEditCursorBlockPos = -1;
            bool editNextItem = editorFlag(textEdit, "editNextItem"),
                 editPrevItem = editorFlag(textEdit, "editPrevItem");
            QTextDocument* doc = textEdit->document();

            if (keyEvent->key() == Qt::Key_Return  && keyEvent->modifiers() == Qt::ControlModifier) {
                emit commitData(textEdit);
                emit closeEditor(textEdit);
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Up) {
                if (doc && editPrevItem && textEdit->textCursor().block() == doc->firstBlock()) {
                    mTextEditCursorAtEnd = true;
                    mTextEditCursorBlockPos = textEdit->textCursor().positionInBlock();
                    emit commitData(textEdit);
                    emit closeEditor(textEdit, QAbstractItemDelegate::EditPreviousItem);
                    return true;
                }
            }
            else if (keyEvent->key() == Qt::Key_Down) {
                if (doc && editNextItem && textEdit->textCursor().block() == doc->lastBlock()) {
                    mTextEditCursorBlockPos = textEdit->textCursor().positionInBlock();
                    emit commitData(textEdit);
                    emit closeEditor(textEdit, QAbstractItemDelegate::EditNextItem);
                    return true;
                }
            }
            /*else if (keyEvent->key() == Qt::Key_Left) {
                if (editPrevItem && textEdit->textCursor().position() == 0) {
                    mTextEditCursorAtEnd = true;
                    emit commitData(textEdit);
                    emit closeEditor(textEdit, QAbstractItemDelegate::EditPreviousItem);
                    return true;
                }
            }
            else if (keyEvent->key() == Qt::Key_Right) {
                if (editNextItem && textEdit->textCursor().position()+1 == doc->characterCount()) {
                    emit commitData(textEdit);
                    emit closeEditor(textEdit, QAbstractItemDelegate::EditNextItem);
                    return true;
                }
            }*/
        }
    }

    return QStyledItemDelegate::eventFilter(watched, event);
}

bool ActionsViewDelegate::editorFlag(QWidget* editor, const char * property) const
{
    QVariant value = editor->property(property);
    if (value.type() == QVariant::Bool)
        return value.toBool();
    return false;
}

ActionsView::ActionsView(QWidget *parent) :
    QListView(parent)
{
    mWriteAction = 0;
    mEditTextMode = false;
    ActionsModel* model = new ActionsModel(this);
    mActionsModel = model;
    this->setModel(model);

    setDragEnabled(true);
    setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDragDropOverwriteMode(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    QStyledItemDelegate* delegate = new ActionsViewDelegate(this);
    setItemDelegate(delegate);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setResizeMode(QListView::Adjust);

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

    QAction* writeAction = new QAction(this);
    writeAction->setShortcut(Qt::Key_Return);
    writeAction->setShortcutContext(Qt::WidgetShortcut);
    addAction(writeAction);

    QAction* modifyActionText = new QAction(this);
    modifyActionText->setShortcut(Qt::CTRL + Qt::Key_Return);
    modifyActionText->setShortcutContext(Qt::WidgetShortcut);
    addAction(modifyActionText);

    connect(mCopyAction, SIGNAL(triggered()), this, SLOT(onCopyAction()));
    connect(mCutAction, SIGNAL(triggered()), this,  SLOT(onCutAction()));
    connect(mPasteAction, SIGNAL(triggered()), this, SLOT(onPasteAction()));
    connect(mDeleteAction, SIGNAL(triggered()), this, SLOT(onDeleteAction()));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onContextMenuRequested(const QPoint&)));

    connect(this, SIGNAL(clicked(const QModelIndex&)), this, SLOT(onItemClicked(const QModelIndex&)));
    connect(delegate, SIGNAL(closeEditor(QWidget*,QAbstractItemDelegate::EndEditHint)), this, SLOT(onEditorClosed(QWidget*, QAbstractItemDelegate::EndEditHint)));
    connect(writeAction, SIGNAL(triggered()), this, SLOT(onWriteAction()));
    connect(modifyActionText, SIGNAL(triggered()), this, SLOT(onModifyActionText()));

    viewport()->installEventFilter(this);
}

void ActionsView::onContextMenuRequested(const QPoint & point)
{
    QMenu menu;
    bool canPaste = this->canPaste();

    if (! selectedIndexes().isEmpty()) {
        menu.addAction(mCopyAction);
        menu.addAction(mCutAction);
        if (canPaste)
            menu.addAction(mPasteAction);
        menu.addSeparator();
        menu.addAction(mDeleteAction);
    }
    else if (canPaste) {
        menu.addAction(mPasteAction);
    }
    menu.exec(mapToGlobal(point));
}

void ActionsView::onDeleteAction()
{
    setAutoScroll(false);
    QModelIndexList indexes = selectedIndexes();
    qSort(indexes);
    for(int i=indexes.size()-1; i >= 0; --i) {
        mActionsModel->removeAction(indexes[i].row(), true);
    }
    setAutoScroll(true);
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
    QModelIndexList indexes = selectedIndexes();
    QModelIndex lastIndex = indexes.isEmpty() ? QModelIndex() : indexes.last();
    int insertRow = lastIndex.row();
    if (actions.isEmpty())
        return;

    Action* firstAction = actions.first();
    int actionsSize = actions.size();
    int actionIndex = insertRow;
    QModelIndex firstIndex = mActionsModel->indexForAction(firstAction);

    if (firstIndex.isValid()) {
        actionIndex = firstIndex.row();
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

        foreach(Action* action, actions) {
            if (action->manager() == mActionsModel->actionManager())
                mActionsModel->removeAction(action);
            else if (action->manager())
                action->manager()->remove(action);
        }

        pasteActionsAt(insertRow, actions, true);
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
    QModelIndexList indexes = selectedIndexes();
    Action* action = 0;
    qSort(indexes);
    for(int i=0; i < indexes.size(); i++) {
        action = mActionsModel->actionForIndex(indexes[i]);
        if (action)
            actions.append(action);
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

void ActionsView::setCurrentAction(Action * action)
{
    if (!mActionsModel)
        return;

    QModelIndex index = mActionsModel->indexForAction(action);
    setCurrentIndex(index);
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
    int i = 0;
    QList<Action*> newActions;
    foreach(Action* action, actions) {
        if (copy)
            action = GameObjectFactory::createAction(action->toJsonObject(), this);

        if (index >= 0)
            mActionsModel->insertAction(index+i, action);
        else
            mActionsModel->appendAction(action);
        newActions.append(action);
        i++;
    }

    if (select)
        selectActions(newActions);
}

void ActionsView::onEditorClosed(QWidget * editor, QAbstractItemDelegate::EndEditHint hint)
{
    if (mWriteAction && mWriteAction == mActionsModel->currentAction() && mWriteAction->editText().isEmpty()) {
        mActionsModel->removeAction(mWriteAction);
    }

    mWriteAction = 0;
    if (hint != QAbstractItemDelegate::EditNextItem && hint != QAbstractItemDelegate::EditPreviousItem) {
        mEditTextMode = false;
        QModelIndex index = currentIndex();
        selectionModel()->clear();
        selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
    }
    else {
        mEditTextMode = true;
    }
}

bool ActionsView::canPaste() const
{
    Clipboard* clipboard = Belle::instance()->clipboard();
    QList<Action*> actions = clipboard->actions();

    if (!actions.isEmpty())
        return true;
    return false;
}

void ActionsView::onWriteAction()
{
    Action* action = GameObjectFactory::createAction(GameObjectMetaType::Dialogue, mActionsModel->actionManager());
    mWriteAction = action;
    addActionItem(action);
    QModelIndex index = mActionsModel->indexForAction(action);
    QListView::edit(index);
}

void ActionsView::addActionItem(Action * action)
{
    QItemSelectionModel* selectionModel = this->selectionModel();
    int row = -1;

    if (selectionModel) {
        QModelIndexList indexes = selectionModel->selectedIndexes();
        if (indexes.size())
            row = indexes.last().row();
    }

    if (row != -1) {
        mActionsModel->insertAction(row+1, action);
    }
    else {
        mActionsModel->appendAction(action);
        scrollToBottom();
    }

    setCurrentAction(action);
}

void ActionsView::onModifyActionText()
{
    Action* action = mActionsModel->actionForIndex(currentIndex());
    if (!action)
        return;

    if (action->isTextEditable())
        QListView::edit(currentIndex());
}

bool ActionsView::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == viewport() && event->type() == QEvent::MouseButtonPress) {
        mEditTextMode = false;
    }

    return QListView::eventFilter(watched, event);
}

bool ActionsView::edit(const QModelIndex &index, EditTrigger trigger, QEvent *event)
{
    if (state() == QAbstractItemView::EditingState)
        return true;

    bool edit = QListView::edit(index, trigger, event);
    if (!edit && mEditTextMode && trigger == QAbstractItemView::CurrentChanged && (index.flags() & Qt::ItemIsEditable) != 0) {
        edit = QListView::edit(index, QAbstractItemView::AllEditTriggers, event);
    }

    return edit;
}
