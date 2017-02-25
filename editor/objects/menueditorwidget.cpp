#include "menueditorwidget.h"

MenuEditorWidget::MenuEditorWidget(QWidget *parent) :
    ObjectGroupEditorWidget(parent)
{
    mChooseNumberOfOptions = new QComboBox(this);
    for(int i=2; i <= MENU_MAX_OPTIONS; i++)
        mChooseNumberOfOptions->addItem(QString::number(i));
    connect(mChooseNumberOfOptions, SIGNAL(currentIndexChanged(int)), this, SLOT(onNumberOfOptionsChanged(int)));

    beginGroup(tr("Menu"), "Menu");
    appendRow(tr("Number of Options"), mChooseNumberOfOptions);
    endGroup();

    mFirstOptionIndex = model()->rowCount();
    ActionManagerButton* actionManagerButton = 0;

    for (int i=0; i < MENU_MAX_OPTIONS; i++) {
        mTextEdits.append(new QLineEdit(this));
//        QTextEdit* conditionEdit = new ConditionTextEdit(this);
//        conditionEdit->installEventFilter(this);
//        conditionEdit->setMaximumHeight(50);
//        mConditionEdits.append(conditionEdit);
        actionManagerButton = new ActionManagerButton(this);
        actionManagerButton->hideAction(GameObjectMetaType::Label);
        mActionButtons.append(actionManagerButton);

        beginGroup(QString("%1 %2").arg(tr("Option")).arg(QString::number(i+1)));
        appendRow(tr("Text"), mTextEdits.last());
        appendRow(tr("Action"), mActionButtons.last());
        //appendRow(tr("Condition"), mConditionEdits.last());
        endGroup();

        if (i >= 2)
            this->setRowHidden(mFirstOptionIndex+i, model()->index(i+1, 0).parent(), true);

        //connect(mConditionEdits.last(), SIGNAL(textChanged()), this, SLOT(onConditionChanged()));
        connect(mTextEdits.last(), SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdited(const QString&)));
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
    foreach(ActionManagerButton* button, mActionButtons)
        button->clear();

    QList<MenuOption*> options = menu->options();
    int optionsSize = options.size();
    int index = optionsSize >= 2 ? optionsSize-2 : 0;
    mChooseNumberOfOptions->setCurrentIndex(index);
    setMenuOptions(menu->options());
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

    menu->setNumberOfOptions(number);
    setMenuOptions(menu->options());
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

bool MenuEditorWidget::eventFilter(QObject * obj, QEvent * ev)
{
    if (obj == mChooseNumberOfOptions && ev->type() == QEvent::Wheel) {
        return true;
    }

    return ObjectGroupEditorWidget::eventFilter(obj, ev);
}

void MenuEditorWidget::setMenuOptions(const QList<MenuOption *> & options)
{
    MenuOption* option = 0;

    setNumberOfOptions(options.size());

    for(int i=0; i < options.size(); i++) {
        option = options.at(i);
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
