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

#include "condition_text_edit.h"
#include "scene_manager.h"
#include "menu.h"
#include "add_action_dialog.h"
#include "interaction.h"

ShowMenuEditorWidget::ShowMenuEditorWidget(QWidget *parent) :
    ActionEditorWidget(parent)
{
    mMenuComboBox = new ObjectComboBox(this);
    connect(mMenuComboBox, SIGNAL(objectChanged(Object*)), this, SLOT(onMenuChanged(Object*)));

    mChooseNumberOfOptions = new QComboBox(this);
    for(int i=2; i <= NUMBER_OF_OPTIONS; i++)
        mChooseNumberOfOptions->addItem(QString::number(i));
    connect(mChooseNumberOfOptions, SIGNAL(currentIndexChanged(int)), this, SLOT(onNumberOfOptionsChanged(int)));
    beginGroup(tr("Show Menu"));
    appendRow(tr("Menu"), mMenuComboBox);
    appendRow(tr("Number of Options"), mChooseNumberOfOptions);
    endGroup();

    mFirstOptionIndex = model()->rowCount();

    for (int i=0; i < NUMBER_OF_OPTIONS; i++) {
        mTextEdits.append(new QLineEdit(this));
//        QTextEdit* conditionEdit = new ConditionTextEdit(this);
//        conditionEdit->installEventFilter(this);
//        conditionEdit->setMaximumHeight(50);
//        mConditionEdits.append(conditionEdit);
        mActionButtons.append(new ActionManagerButton(this));

        beginGroup(QString("%1 %2").arg(tr("Option")).arg(QString::number(i+1)));
        appendRow(tr("Text"), mTextEdits.last());
        appendRow(tr("Action"), mActionButtons.last());
        //appendRow(tr("Condition"), mConditionEdits.last());
        endGroup();

        if (i >= 2)
            this->setRowHidden(mFirstOptionIndex+i, model()->index(i+1, 0).parent(), true);

        //connect(mConditionEdits.last(), SIGNAL(textChanged()), this, SLOT(onConditionChanged()));
        connect(mTextEdits.last(), SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdited(const QString&)));
        connect(mActionButtons.last(), SIGNAL(managingFinished()), this, SLOT(updateShowMenuText()));
    }

    resizeColumnToContents(0);
}

void ShowMenuEditorWidget::updateData(GameObject * action)
{
    ActionEditorWidget::updateData(action);
    ShowMenu* showMenu = qobject_cast<ShowMenu*>(action);
    if (! showMenu)
        return;

    QList<GameObject*> resources = ResourceManager::instance()->objects(GameObjectMetaType::Menu);
    mMenuComboBox->clear();
    foreach(GameObject* menu, resources) {
        if (menu)
            mMenuComboBox->addObject(qobject_cast<Menu*>(menu));
    }

    mMenuComboBox->setCurrentIndex(-1);
    Object* obj = showMenu->sceneObject();
    if (obj) {
        mMenuComboBox->setCurrentObject(qobject_cast<Menu*>(obj->resource()));
    }

    //clear all data
    foreach(QLineEdit* lineEditor, mTextEdits)
        lineEditor->clear();
    foreach(QTextEdit* conditionEditor, mConditionEdits)
        conditionEditor->clear();
    foreach(ActionManagerButton* button, mActionButtons)
        button->clear();

    if (! showMenu->sceneObject())
        return;

    Menu* menu = static_cast<Menu*>(showMenu->sceneObject());
    MenuOption* option = 0;
    QList<Object*> objects = menu->objects();

    int index = menu->objects().size() >= 2 ? menu->objects().size()-2 : 0;
    mChooseNumberOfOptions->setCurrentIndex(index);
    setNumberOfOptions(menu->options().size());

    for(int i=0; i < objects.size(); i++) {
        option = menu->optionAt(i);
        if (! option)
            continue;

        if (i < mTextEdits.size())
            mTextEdits[i]->setText(option->text());

        if (i < mConditionEdits.size())
            mConditionEdits[i]->setPlainText(option->condition());

        if (i < mActionButtons.size())
            mActionButtons[i]->setActionManager(option->actionManager());
    }
}

void ShowMenuEditorWidget::onTextEdited(const QString & text)
{
    ShowMenu* showMenu = qobject_cast<ShowMenu*>(mGameObject);
    if (!showMenu || !showMenu->sceneObject())
        return;

    QLineEdit* lineEdit = static_cast<QLineEdit*>(sender());

    int index = mTextEdits.indexOf(lineEdit);
    if (index == -1)
        return;

    Menu* menu = static_cast<Menu*>(showMenu->sceneObject());
    if (menu) {
        MenuOption* option = menu->optionAt(index);
        if (option)
            option->setText(text);
    }
}

void ShowMenuEditorWidget::onNumberOfOptionsChanged(int index)
{
    ShowMenu* showMenu = qobject_cast<ShowMenu*>(mGameObject);
    if (! sender() || ! showMenu || ! showMenu->sceneObject())
        return;

    Menu* menu = static_cast<Menu*>(showMenu->sceneObject());
    QComboBox *comboBox = static_cast<QComboBox*>(sender());

    bool ok;
    int number = comboBox->currentText().toInt(&ok);
    if (! ok)
        return;

    this->setNumberOfOptions(number);
    menu->setNumberOfOptions(number);
    _updateTexts(menu);
}

void ShowMenuEditorWidget::_updateTexts(Menu* menu)
{
    MenuOption* option = 0;
    for(int i=0; i < mTextEdits.size(); i++) {
        option = menu->optionAt(i);
        if (option)
            mTextEdits.at(i)->setText(option->text());
    }
}

void ShowMenuEditorWidget::onConditionChanged()
{
    ShowMenu* showMenu = qobject_cast<ShowMenu*>(mGameObject);
    if (! sender() || ! showMenu || ! showMenu->sceneObject())
        return;

    QTextEdit* textEdit = static_cast<QTextEdit*>(sender());
    int index = mConditionEdits.indexOf(textEdit);

    if (index != -1) {
        Menu* menu = static_cast<Menu*>(showMenu->sceneObject());
        MenuOption* option = menu->optionAt(index);
        if (option)
            option->setCondition(textEdit->toPlainText());
    }
}

void ShowMenuEditorWidget::setNumberOfOptions(int number)
{
    for(int i=mFirstOptionIndex; i < mFirstOptionIndex+number; i++) {
        this->setRowHidden(i, model()->index(i, 0).parent(), false);
    }

    for(int i=mFirstOptionIndex+number; i < model()->rowCount(); i++) {
        this->setRowHidden(i, model()->index(i, 0).parent(), true);
    }
}

void ShowMenuEditorWidget::onMenuChanged(Object * obj)
{
    Menu* menu = qobject_cast<Menu*>(obj);
    ShowMenu* showMenu = qobject_cast<ShowMenu*>(mGameObject);
    if (!menu || !showMenu)
        return;

    showMenu->setMenuResource(menu);
}

void ShowMenuEditorWidget::updateShowMenuText()
{
    ShowMenu* showMenu = qobject_cast<ShowMenu*>(mGameObject);
    if (!showMenu)
        return;

    showMenu->updateDisplayText();
}
