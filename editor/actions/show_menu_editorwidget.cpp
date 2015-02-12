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

#include "show_menu_editorwidget.h"

#include <QtDebug>

#include "condition_text_edit.h"
#include "scene_manager.h"
#include "menu.h"
#include "add_action_dialog.h"
#include "interaction.h"

ShowMenuEditorWidget::ShowMenuEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{

    mChooseNumberOfOptions = new QComboBox(this);
    for(int i=2; i <= NUMBER_OF_OPTIONS; i++)
        mChooseNumberOfOptions->addItem(QString::number(i));
    connect(mChooseNumberOfOptions, SIGNAL(currentIndexChanged(int)), this, SLOT(onNumberOfOptionsChanged(int)));
    appendRow(tr("Number of Options"), mChooseNumberOfOptions);

    for (int i=0; i < NUMBER_OF_OPTIONS; i++) {
        mTextEdits.append(new QLineEdit(this));
        QTextEdit* conditionEdit = new ConditionTextEdit(this);
        conditionEdit->installEventFilter(this);
        conditionEdit->setMaximumHeight(50);
        mConditionEdits.append(conditionEdit);
        mEventChoosers.append(new ComboBox(this));

        beginGroup(QString("%1 %2").arg(tr("Option")).arg(QString::number(i+1)));
        appendRow(tr("Text"), mTextEdits.last());
        appendRow(tr("Action"), mEventChoosers.last());
        appendRow(tr("Condition"), mConditionEdits.last());
        endGroup();

        if (i >= 2)
            this->setRowHidden(i+1, model()->index(i+1, 0).parent(), true);

        connect(mConditionEdits.last(), SIGNAL(textChanged()), this, SLOT(onConditionChanged()));
        connect(mTextEdits.last(), SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdited(const QString&)));
        connect(mEventChoosers.last(), SIGNAL(addItemActivated()), this, SLOT(onAddItemActivated()));
        connect(mEventChoosers.last(), SIGNAL(itemRemoved(int)), this, SLOT(onItemRemoved(int)));
        connect(mEventChoosers.last(), SIGNAL(itemActivated(int)), this, SLOT(onItemActivated(int)));
    }

    resizeColumnToContents(0);
}

void ShowMenuEditorWidget::updateData(Action * action)
{
    ShowMenu* showMenu = qobject_cast<ShowMenu*>(action);
    if (! showMenu)
        return;

    ActionEditorWidget::updateData(action);
    mAction = 0;

    //clear all data
    foreach(QLineEdit* lineEditor, mTextEdits)
        lineEditor->clear();
    foreach(QTextEdit* conditionEditor, mConditionEdits)
        conditionEditor->clear();
    foreach(ComboBox* eventChooser, mEventChoosers)
        eventChooser->clear();

    if (! showMenu->sceneObject())
        return;


    Menu* menu = static_cast<Menu*>(showMenu->sceneObject());
    QList<Object*> objects = menu->objects();
    QList<Action*> actions;

    int index = menu->objects().size() >= 2 ? menu->objects().size()-2 : 0;
    mChooseNumberOfOptions->setCurrentIndex(index);
    setNumberOfOptions(menu->objects().size());

    for(int i=0; i < objects.size(); i++) {
        if (i < mTextEdits.size())
            mTextEdits[i]->setText(menu->optionText(i));

        if (i < mConditionEdits.size())
            mConditionEdits[i]->setPlainText(menu->condition(i));

        actions = menu->optionActions(i);
        if (i < mEventChoosers.size()) {
            for(int j=0; j < actions.size(); j++)
                mEventChoosers[i]->addItem(actions[j]->icon(), actions[j]->toString());
        }
    }

    mAction = action;
}

void ShowMenuEditorWidget::onTextEdited(const QString & text)
{
    if (!mAction || !mAction->sceneObject())
        return;

    QLineEdit* lineEdit = static_cast<QLineEdit*>(sender());

    int index = mTextEdits.indexOf(lineEdit);
    if (index == -1)
        return;

    Menu* menu = static_cast<Menu*>(mAction->sceneObject());
    menu->setOptionText(index, text);
}

void ShowMenuEditorWidget::onAddItemActivated()
{
    if (! mAction || ! mAction->sceneObject())
        return;

    Menu* menu = static_cast<Menu*>(mAction->sceneObject());

    AddActionDialog dialog(Interaction::MouseRelease, this);
    dialog.exec();

    if (dialog.result() == QDialog::Accepted && dialog.selectedAction()) {
        Action* action = dialog.selectedAction();

        ComboBox *comboBox = qobject_cast<ComboBox*>(sender());
        if(comboBox && action) {
            comboBox->addItem(action->icon(), action->toString());
            int index = mEventChoosers.indexOf(comboBox);
            menu->appendActionToOption(index, action);
        }
    }
}

void ShowMenuEditorWidget::onItemRemoved(int actionIndex)
{
    if (! mAction || ! mAction->sceneObject())
        return;

    Menu* menu = static_cast<Menu*>(mAction->sceneObject());
    ComboBox *comboBox = qobject_cast<ComboBox*>(sender());
    if (! menu || ! comboBox)
        return;

    int optIndex = mEventChoosers.indexOf(comboBox);
    menu->removeActionFromOption(optIndex, actionIndex, true);

}

void ShowMenuEditorWidget::onItemActivated(int actionIndex)
{
    if (! mAction || ! mAction->sceneObject())
        return;

    Menu* menu = static_cast<Menu*>(mAction->sceneObject());
    if (! menu)
        return;

    int optIndex = widgetIndex(sender());
    Object* object = menu->object(optIndex);
    if (! object)
        return;

    QList<Action*> actions = object->actionsForEvent(Interaction::MouseRelease);
    if (actionIndex < 0 || actionIndex >= actions.size())
        return;

    AddActionDialog dialog(actions[actionIndex]);
    dialog.exec();

    mEventChoosers[optIndex]->setItemText(actionIndex, actions[actionIndex]->toString());

}

int ShowMenuEditorWidget::widgetIndex(QObject* objectComboBox)
{
    ComboBox *comboBox = qobject_cast<ComboBox*>(objectComboBox);
    if (comboBox)
      return mEventChoosers.indexOf(comboBox);
    return -1;
}

void ShowMenuEditorWidget::onNumberOfOptionsChanged(int index)
{
    if (! sender() || ! mAction || ! mAction->sceneObject())
        return;

    Menu* menu = static_cast<Menu*>(mAction->sceneObject());
    QComboBox *comboBox = static_cast<QComboBox*>(sender());

    bool ok;
    int number = comboBox->currentText().toInt(&ok);
    if (! ok)
        return;

    this->setNumberOfOptions(number);
    menu->setNumberOfOptions(number);
}

void ShowMenuEditorWidget::onConditionChanged()
{
    if (! sender() || ! mAction || ! mAction->sceneObject())
        return;

    QTextEdit* textEdit = static_cast<QTextEdit*>(sender());
    int index = mConditionEdits.indexOf(textEdit);

    if (index != -1) {
        Menu* menu = static_cast<Menu*>(mAction->sceneObject());
        menu->setCondition(index, textEdit->toPlainText());
    }
}

void ShowMenuEditorWidget::setNumberOfOptions(int number)
{
    for(int i=1; i < number+1; i++) {
        this->setRowHidden(i, model()->index(i, 0).parent(), false);
    }

    for(int i=number+1; i < model()->rowCount(); i++) {
        this->setRowHidden(i, model()->index(i, 0).parent(), true);
    }
}
