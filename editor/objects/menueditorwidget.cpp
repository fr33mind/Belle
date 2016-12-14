#include "menueditorwidget.h"
#include "add_action_dialog.h"

MenuEditorWidget::MenuEditorWidget(QWidget *parent) :
    ObjectGroupEditorWidget(parent)
{
    mButtonComboBox = new ObjectComboBox(this);
    connect(mButtonComboBox, SIGNAL(objectChanged(Object*)), this, SLOT(onButtonChanged(Object*)));

    mChooseNumberOfOptions = new QComboBox(this);
    for(int i=2; i <= MENU_MAX_OPTIONS; i++)
        mChooseNumberOfOptions->addItem(QString::number(i));
    connect(mChooseNumberOfOptions, SIGNAL(currentIndexChanged(int)), this, SLOT(onNumberOfOptionsChanged(int)));

    beginGroup(tr("Menu"), "Menu");
    appendRow(tr("Button"), mButtonComboBox);
    appendRow(tr("Number of Options"), mChooseNumberOfOptions);
    endGroup();

    mFirstOptionIndex = model()->rowCount();

    for (int i=0; i < MENU_MAX_OPTIONS; i++) {
        mTextEdits.append(new QLineEdit(this));
//        QTextEdit* conditionEdit = new ConditionTextEdit(this);
//        conditionEdit->installEventFilter(this);
//        conditionEdit->setMaximumHeight(50);
//        mConditionEdits.append(conditionEdit);
        mEventChoosers.append(new ComboBox(this));

        beginGroup(QString("%1 %2").arg(tr("Option")).arg(QString::number(i+1)));
        appendRow(tr("Text"), mTextEdits.last());
        appendRow(tr("Action"), mEventChoosers.last());
        //appendRow(tr("Condition"), mConditionEdits.last());
        endGroup();

        if (i >= 2)
            this->setRowHidden(mFirstOptionIndex+i, model()->index(i+1, 0).parent(), true);

        //connect(mConditionEdits.last(), SIGNAL(textChanged()), this, SLOT(onConditionChanged()));
        connect(mTextEdits.last(), SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdited(const QString&)));
        connect(mEventChoosers.last(), SIGNAL(addItemActivated()), this, SLOT(onAddItemActivated()));
        connect(mEventChoosers.last(), SIGNAL(itemRemoved(int)), this, SLOT(onItemRemoved(int)));
        connect(mEventChoosers.last(), SIGNAL(itemActivated(int)), this, SLOT(onItemActivated(int)));
    }

    resizeColumnToContents(0);

    mChooseNumberOfOptions->installEventFilter(this);
}

void MenuEditorWidget::updateData(GameObject * object)
{
    ObjectGroupEditorWidget::updateData(object);
    Menu* menu = qobject_cast<Menu*>(object);
    if (! menu)
        return;

    //clear all data
    foreach(QLineEdit* lineEditor, mTextEdits)
        lineEditor->clear();
    foreach(QTextEdit* conditionEditor, mConditionEdits)
        conditionEditor->clear();
    foreach(ComboBox* eventChooser, mEventChoosers)
        eventChooser->clear();

    MenuOption* option = 0;
    QList<Object*> objects = menu->objects();
    QList<Action*> actions;

    QList<GameObject*> btnResources = ResourceManager::instance()->objects(GameObjectMetaType::Button);
    mButtonComboBox->clear();
    foreach(GameObject* btn, btnResources) {
        if (btn)
            mButtonComboBox->addObject(qobject_cast<Button*>(btn));
    }

    mButtonComboBox->setCurrentObject(menu->buttonResource());

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

        actions = option->actions();
        if (i < mEventChoosers.size()) {
            for(int j=0; j < actions.size(); j++) {
                const GameObjectMetaType* metatype = GameObjectMetaType::metaType(actions[j]->type());
                const QIcon typeIcon = metatype ? metatype->icon() : QIcon();
                mEventChoosers[i]->addItem(typeIcon, actions[j]->toString());
            }
        }
    }
}

void MenuEditorWidget::onNumberOfOptionsChanged(int index)
{
    Menu* menu = qobject_cast<Menu*>(mGameObject);
    QObject* _sender = sender();
    if (!_sender || !menu)
        return;

    QComboBox *comboBox = static_cast<QComboBox*>(_sender);

    bool ok;
    int number = comboBox->currentText().toInt(&ok);
    if (! ok)
        return;

    setNumberOfOptions(number);
    menu->setNumberOfOptions(number);
    _updateTexts(menu);
}

void MenuEditorWidget::setNumberOfOptions(int number)
{
    for(int i=mFirstOptionIndex; i < mFirstOptionIndex+number; i++) {
        this->setRowHidden(i, model()->index(i, 0).parent(), false);
    }

    for(int i=mFirstOptionIndex+number; i < model()->rowCount(); i++) {
        this->setRowHidden(i, model()->index(i, 0).parent(), true);
    }
}

void MenuEditorWidget::_updateTexts(Menu* menu)
{
    MenuOption* option = 0;
    for(int i=0; i < mTextEdits.size(); i++) {
        option = menu->optionAt(i);
        if (option)
            mTextEdits.at(i)->setText(option->text());
    }
}

void MenuEditorWidget::onTextEdited(const QString & text)
{
    Menu* menu = qobject_cast<Menu*>(mGameObject);
    if (!menu)
        return;

    QLineEdit* lineEdit = static_cast<QLineEdit*>(sender());

    int index = mTextEdits.indexOf(lineEdit);
    if (index == -1)
        return;

    MenuOption* option = menu->optionAt(index);
    if (option)
        option->setText(text);
}

void MenuEditorWidget::onAddItemActivated()
{
    Menu* menu = qobject_cast<Menu*>(mGameObject);
    if (!menu)
        return;

    AddActionDialog dialog(qobject_cast<QObject*>(mGameObject), this);
    dialog.exec();

    if (dialog.result() == QDialog::Accepted && dialog.selectedAction()) {
        Action* action = dialog.selectedAction();

        ComboBox *comboBox = qobject_cast<ComboBox*>(sender());
        if(comboBox && action) {
            const QIcon typeIcon = GameObjectMetaType::icon(action->type());
            comboBox->addItem(typeIcon, action->toString());
            int index = mEventChoosers.indexOf(comboBox);
            MenuOption* option = menu->optionAt(index);
            if (option)
                option->addAction(action);
        }
    }
}

void MenuEditorWidget::onItemRemoved(int actionIndex)
{
    Menu* menu = qobject_cast<Menu*>(mGameObject);
    ComboBox *comboBox = qobject_cast<ComboBox*>(sender());
    if (! menu || ! comboBox)
        return;

    int optIndex = mEventChoosers.indexOf(comboBox);
    MenuOption* option = menu->optionAt(optIndex);
    if (option)
        option->removeActionAt(actionIndex);
}

void MenuEditorWidget::onItemActivated(int actionIndex)
{
    Menu* menu = qobject_cast<Menu*>(mGameObject);
    if (!menu)
        return;

    int optIndex = widgetIndex(sender());
    MenuOption* option = menu->optionAt(optIndex);
    if (! option)
        return;

    QList<Action*> actions = option->actions();
    if (actionIndex < 0 || actionIndex >= actions.size())
        return;

    AddActionDialog dialog(actions[actionIndex]);
    dialog.exec();

    mEventChoosers[optIndex]->setItemText(actionIndex, actions[actionIndex]->toString());
}

int MenuEditorWidget::widgetIndex(QObject* objectComboBox)
{
    ComboBox *comboBox = qobject_cast<ComboBox*>(objectComboBox);
    if (comboBox)
      return mEventChoosers.indexOf(comboBox);
    return -1;
}

void MenuEditorWidget::onButtonChanged(Object * obj)
{
    Button* btn = qobject_cast<Button*>(obj);
    Menu* menu = qobject_cast<Menu*>(mGameObject);
    if (!menu || !btn)
        return;

    menu->setButtonResource(btn);
}

bool MenuEditorWidget::eventFilter(QObject * obj, QEvent * ev)
{
    if (obj == mChooseNumberOfOptions && ev->type() == QEvent::Wheel) {
        return true;
    }

    return ObjectGroupEditorWidget::eventFilter(obj, ev);
}
