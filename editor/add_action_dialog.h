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

#ifndef ADD_ACTION_DIALOG_H
#define ADD_ACTION_DIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QDialogButtonBox>

#include "action_info_manager.h"
#include "action_catalog_widget.h"

class AddActionDialog : public QDialog
{
    Q_OBJECT

    int mPreviousIndex;
    QList<Action*> mActionsCatalog;
    QHash<Action*, ActionEditorWidget*> mActionsToEditor;
    QWidget * mPropertiesEditorParentWidget;
    QComboBox * mActionsComboBox;
    ActionCatalogWidget *mActionCatalogWidget;
    Action* mCurrentAction;
    QDialogButtonBox* mButtonBox;
    bool mEditMode;
    QObject* mActionParent;

public:
    AddActionDialog(QObject*, QWidget *parent = 0);
    AddActionDialog(Action*, QWidget *parent = 0);
    ~AddActionDialog();
    Action* selectedAction();

signals:


protected:
    void closeEvent(QCloseEvent *);
    void switchPropertyWidget(QWidget*);

public slots:
    void onActionChanged(int);
    void onClose(int);
    void onNewAction(Action*);

private:
    void init();

};

#endif // ADD_ACTION_DIALOG_H
