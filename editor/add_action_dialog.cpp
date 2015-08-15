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

#include "add_action_dialog.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QComboBox>
#include <QDialogButtonBox>

#include "gotolabel.h"
#include "action_info_manager.h"
#include "scene_manager.h"
#include "belle.h"
#include "editorwidgetfactory.h"

AddActionDialog::AddActionDialog(QObject* actionParent, QWidget *parent) :
    QDialog(parent)
{
    init();

    mActionParent = actionParent;
    mActionCatalogWidget = new ActionCatalogWidget(this);
    mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    setLayout(new QVBoxLayout(this));
    layout()->addWidget(mActionCatalogWidget);
    layout()->addWidget(mPropertiesEditorParentWidget);
    layout()->addWidget(mButtonBox);

    //mActionsComboBox = new QComboBox(this);
    //layout()->addWidget(mActionsComboBox);

    /*mActionsCatalog = ActionInfoManager::actions();
    mActionsComboBox->addItem(tr("Do Nothing"));
    for(int i=mActionsCatalog.size()-1; i >= 0; i--) {
        if (! mActionsCatalog[i]->supportsEvent(event))
            mActionsCatalog.removeAt(i);
        else {
            if (mActionsCatalog[i]->editorWidget())
                mActionsCatalog[i]->editorWidget()->setFilters(QStringList() << "Object");
        }
    }

    for(int i=0; i < mActionsCatalog.size(); i++)
        mActionsComboBox->addItem(mActionsCatalog[i]->icon(), mActionsCatalog[i]->name());*/

    connect(mActionCatalogWidget, SIGNAL(newAction(Action*)), this, SLOT(onNewAction(Action*)));
    //connect(mActionsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onActionChanged(int)));
    connect(mButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(mButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(this, SIGNAL(finished(int)), this, SLOT(onClose(int)));
}

AddActionDialog::AddActionDialog(Action* action, QWidget *parent) :
    QDialog(parent)
{
    init();
    if (action)
        mActionParent = action->parent();
    setWindowTitle(tr("Edit Action"));
    resize(640, 320);
    mEditMode = true;
    mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this);
    setLayout(new QVBoxLayout(this));
    layout()->addWidget(mPropertiesEditorParentWidget);
    layout()->addWidget(mButtonBox);
    onNewAction(action);

    connect(mButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(this, SIGNAL(finished(int)), this, SLOT(onClose(int)));
}

void AddActionDialog::init()
{
    setWindowTitle(tr("Choose Action"));
    resize(320, 240);
    mEditMode = false;
    mActionParent = 0;
    mPreviousIndex = 0;
    mCurrentAction = 0;
    mActionsComboBox = 0;
    mActionCatalogWidget = 0;
    mButtonBox = 0;
    mPropertiesEditorParentWidget = new QWidget(this);
    mPropertiesEditorParentWidget->setLayout(new QVBoxLayout);

}

AddActionDialog::~AddActionDialog()
{
    if (mEditMode)
        return;

    for(int i=0; i < mActionsCatalog.size(); i++) {
        if (mActionsCatalog[i] != mCurrentAction)
            mActionsCatalog[i]->deleteLater();
    }
    mActionsCatalog.clear();

    if (mCurrentAction && this->result() != QDialog::Accepted)
        mCurrentAction->deleteLater();
}

void AddActionDialog::onActionChanged(int index)
{
    if (index < 0)
        return;

    /*QComboBox * comboBox = static_cast<QComboBox*>(sender());

    if (mPreviousIndex > 0) {
        mPropertiesEditorParentWidget->layout()->removeWidget(mActionsCatalog[mPreviousIndex-1]->editorWidget());
        if (mActionsCatalog[mPreviousIndex-1]->editorWidget())
            mActionsCatalog[mPreviousIndex-1]->editorWidget()->setParent(0);
    }

    if (index > 0) {
        if (mActionsCatalog[index-1]->editorWidget()) {
            mPropertiesEditorParentWidget->layout()->addWidget(mActionsCatalog[index-1]->editorWidget());
            mActionsCatalog[index-1]->editorWidget()->updateData(mActionsCatalog[index-1]);
        }
    }
        //layout()->addWidget(mActionsCatalog[index-1]->editorWidget());
    mPreviousIndex = comboBox->currentIndex();*/
}

void AddActionDialog::closeEvent(QCloseEvent * event)
{
    onClose(0);
}

void AddActionDialog::onClose(int c)
{
    switchPropertyWidget(0);


    /*for(int i=0; i < mActionsCatalog.size(); i++) {
        if (mActionsCatalog[i]->editorWidget())
            mActionsCatalog[i]->editorWidget()->setFilters(QStringList());
    }*/
}

Action* AddActionDialog::selectedAction()
{
    //if (mActionsComboBox->currentIndex() > 0)
    //    return mActionsCatalog[mActionsComboBox->currentIndex()-1];
    return mCurrentAction;
}

void AddActionDialog::switchPropertyWidget(QWidget* widget)
{
    QLayout *layout = mPropertiesEditorParentWidget->layout();
    QLayoutItem *item;
    if (! layout)
        return;

    while((item = layout->takeAt(0)) != 0)
        if (item && item->widget()) {
            item->widget()->setParent(0);
            item->widget()->hide();
        }

    if (widget) {
        layout->addWidget(widget);
        if (widget->isHidden())
            widget->show();
    }
}


void AddActionDialog::onNewAction(Action * action)
{
    bool contains = false;
    Action* _action = 0;
    int index = 0;
    GameObjectEditorWidget* editor = 0;
    /*QHashIterator<Action*, ActionEditorWidget*> it(mActionsCatalog);
    while(it.hasNext()) {
        it.next();
        _action = it.key();
        if (action->type() == _action->type()) {
            contains = true;
            break;
        }
    }*/

    if (! action)
        return;

    foreach(_action, mActionsCatalog) {
        if (action->type() == _action->type()){
            contains = true;
            break;
        }
    }

    if (contains) {
        action->deleteLater();
        action = _action;
    }
    else {
        action->setParent(mActionParent);
        mActionsCatalog.append(action);
        //mActionsCatalog.insert(action, action->editorWidget()->copy());
    }

    mCurrentAction = action;
    editor = EditorWidgetFactory::editorWidget(action->type());
    switchPropertyWidget(editor);
    if (editor)
        editor->setGameObject(action);
    if (mActionCatalogWidget)
        resize(640, 480);
}
